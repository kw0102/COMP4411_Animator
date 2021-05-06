#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "modelerui.h"
#include <FL/gl.h>
#include <Fl/glu.h>
#include "camera.h"

#include "modelerglobals.h"
#include "bitmap.h"

#include "Spongebob.h"
#include "IK.h"

#include "LSystem.h"

#define BODY_COLOR 1.0f, 1.0f, 0.0f
#define WHITE_COLOR 1.0f, 1.0f, 1.0f

Mat4d getModelViewMatrix()
{
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Mat4d matMV(m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]);

	return matMV.transpose(); // convert to row major
}


// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSpongebobModel(int x, int y, int w, int h, char* label)
{
	return new SpongebobModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void SpongebobModel::draw()
{
	// Load texture
	if (!loadedTexture)
		loadTexture();

	// Load DL
	if (!hfDL)
		initDL();

	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	// CHeck if animating
	bool animating = ModelerApplication::Instance()->m_ui->m_bAnimating;

	// dramatic lighting
	glEnable(GL_LIGHTING);
	GLfloat lightPos[4] = {VAL(LIGHT_X), VAL(LIGHT_Y), VAL(LIGHT_Z), 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	double lhux = VAL(LHAND_UPPER_X);
	double lhuy = VAL(LHAND_UPPER_Y);
	double lhuz = VAL(LHAND_UPPER_Z);
	double lhl = VAL(LHAND_LOWER);

	double rhux = VAL(RHAND_UPPER_X);
	double rhuy = VAL(RHAND_UPPER_Y);
	double rhuz = VAL(RHAND_UPPER_Z);
	double rhl = VAL(RHAND_LOWER);

	double llux = VAL(LLEG_UPPER_X);
	double lluy = VAL(LLEG_UPPER_Y);
	double lluz = VAL(LLEG_UPPER_Z);
	double lll = VAL(LHAND_LOWER);

	double rlux = VAL(RLEG_UPPER_X);
	double rluy = VAL(RLEG_UPPER_Y);
	double rluz = VAL(RLEG_UPPER_Z);
	double rll = VAL(RLEG_LOWER);



	// Inverse Kinematics
	if (VAL(IK_LIMB))
	{
		IK::setLimb(VAL(IK_LIMB) - 1);
		IK::setTarget(Vec3d(VAL(IK_TARGET_X), VAL(IK_TARGET_Y), VAL(IK_TARGET_Z)));
		IK::setConstraints(VAL(IK_MIN), VAL(IK_MAX));
		IK::solve();

		if (false)
		{
			glPushMatrix();
			Vec3d p = Vec3d(VAL(IK_TARGET_X), VAL(IK_TARGET_Y), VAL(IK_TARGET_Z));
			glTranslated(p[0], p[1], p[2]);
			setDiffuseColor(0, 0, 1);
			drawSphere(0.5f);
			glPopMatrix();
		}

		int limb = (int)VAL(IK_LIMB);

		switch (limb)
		{
		case 1:
			IK::apply(lhux, lhuy, lhuz, lhl); break;
		case 2:
			IK::apply(rhux, rhuy, rhuz, rhl); break;
		case 3:
			IK::apply(rlux, rluy, rluz, rll); break;
		case 4:
			IK::apply(rlux, rluy, rluz, rll); break;
		}

		


	}
	Mat4d cameraMatrix = getModelViewMatrix();

	skybox();

	fishes->setTransformMatrix(cameraMatrix.inverse() * getModelViewMatrix());

	if (hfDL)
	{
		glPushMatrix();
		glTranslated(0, -7.5, 0);
		glScaled(2, 4, 2);
		glCallList(hfDL);
		glPopMatrix();
	}

	/*
	*  Jelly fish
	*/

	// draw the jellyfish
	glPushMatrix();
	glTranslated(VAL(IK_TARGET_X), VAL(IK_TARGET_Y), VAL(IK_TARGET_Z));
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(1.5, 1, 1);
	//glTranslated(-3, 4, 1);
	drawSphere(0.5);
	glRotated(90, 1.0, 0, 0);
	drawCylinder(0.4, 0.5, 0.5);

		//legs
		glPushMatrix();
		glTranslated(-0.4, 0, 0);
		drawCylinder(1, .1f, .1f);
		glTranslated(0, 0, 1);
		drawSphere(.1f);
		glRotated(VAL(JELLY_MOVE), 0.0, 1.0, 0.0);
		drawCylinder(0.5, .1f, .1f);
		glTranslated(0, 0, 0.5);
		drawSphere(.1f);
		glPopMatrix();

		glPushMatrix();
		drawCylinder(1, .1f, .1f);
		glTranslated(0, 0, 1);
		drawSphere(.1f);
		glRotated(VAL(JELLY_MOVE), 0.0, 1.0, 0.0);
		drawCylinder(0.5, .1f, .1f);
		glTranslated(0, 0, 0.5);
		drawSphere(.1f);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.4, 0, 0);
		drawCylinder(1, .1f, .1f);
		glTranslated(0, 0, 1);
		drawSphere(.1f);
		glRotated(VAL(JELLY_MOVE), 0.0, 1.0, 0.0);
		drawCylinder(0.5, .1f, .1f);
		glTranslated(0, 0, 0.5);
		drawSphere(.1f);
		glPopMatrix();

	glPopMatrix();


	/**************************/

	// draw the spongebob
	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

		// torso
		glPushMatrix();
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(BODY_COLOR);
		glTranslated(-2, -2, -2);
		drawBox(4.0, VAL(HEIGHT), 2.0);
		glPopMatrix();
  

		//outline of spongebob
		if (true) {
			glPushMatrix();
			glTranslated(-2, -1.5, -2);
			glScaled(0.05, 0.125, 1);
			drawCylinder(2.0, 4.0, 4.0);

			for (int i = 0; i < 3; i++) {
				glTranslated(0, VAL(HEIGHT) * 2, 0);
				drawCylinder(2.0, VAL(HEIGHT), VAL(HEIGHT));
			}

			glTranslated(4.0 / 0.05, 0, 0);
			for (int i = 0; i < 4; i++) {
				drawCylinder(2.0, VAL(HEIGHT), 4.0);
				glTranslated(0, -VAL(HEIGHT) * 2, 0);
			}
			glTranslated(-4.0 * 3, VAL(HEIGHT) / 0.125 + VAL(HEIGHT), 0);

			glScaled(2.3, 0.4, 1);
			for (int i = 0; i < 4; i++) {
				drawCylinder(2.0, VAL(HEIGHT), VAL(HEIGHT));
				glTranslated(-4.0 * 2, 0, 0);
			}

			glPopMatrix();
		}

		/*
		*  Gary the Snail
		*/

		glPushMatrix();
		setDiffuseColor(1, 0.5, 0.6);
		glTranslated(-3, -1.5, -3);
		glTranslated(4, VAL(HEIGHT), 2);
		glTranslated(VAL(SNAIL_MOVE), 0, 0);
		//drawSphere(1);
		glPushMatrix();
		glScaled(0.35, 0.35, 0.35);
		snailShell.draw();
		glPopMatrix();

		//pattern
		glPushMatrix();
		setDiffuseColor(1, 0, 0);
		glTranslated(0.3, 0, 0.9);
		glRotated(90, 1, 0, 0);
		drawCylinder(0.7, 0.1, 0.1);
		glTranslated(-0.25, -0.35, -0.1);
		drawSphere(0.5);
		setDiffuseColor(1, 0.5, 0.6);
		glTranslated(0, 0.25, 0);
		drawSphere(0.3);
		glPopMatrix();


		glPushMatrix();
		setDiffuseColor(1, 0, 0);
		glTranslated(0.3, 0, -0.9);
		glRotated(90, 1, 0, 0);
		drawCylinder(0.7, 0.1, 0.1);
		glTranslated(-0.25, 0.35, -0.1);
		drawSphere(0.5);
		setDiffuseColor(1, 0.5, 0.6);
		glTranslated(0, -0.25, 0);
		drawSphere(0.3);
		glPopMatrix();

		glPushMatrix();
		setDiffuseColor(0.1, 0.6, 0.7);
		glTranslated(0.1, 0.6, 0.6);
		drawSphere(0.2);
		glTranslated(0.5, -0.2, -0.1);
		drawSphere(0.2);
		glTranslated(-1, 0, 0.1);
		drawSphere(0.2);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0.1, 0.6, -0.6);
		drawSphere(0.2);
		glTranslated(0.5, -0.2, 0.1);
		drawSphere(0.2);
		glTranslated(-1, 0, -0.1);
		drawSphere(0.2);
		glPopMatrix();

		//body
		setDiffuseColor(0.1, 0.6, 0.7);
		glTranslated(0, -0.5, 0);
		glRotated(90, 0, 1, 0);
		drawCylinder(2, 0.4, 0.4);
		glTranslated(-0.2, 0, 2);
		glRotated(-90, 1, 0, 0);
		drawCylinder(1, 0.1, 0.1);
		glTranslated(0.4, 0, 0);
		drawCylinder(1, 0.1, 0.1);

		//eyes
		setDiffuseColor(1, 1, 1);
		glTranslated(0, 0, 1);
		drawSphere(0.2);
		glTranslated(-0.4, 0, 0);
		drawSphere(0.2);
		glTranslated(0, -0.18, 0);
		setDiffuseColor(1, 0, 0);
		drawSphere(0.1);
		glTranslated(0.4, 0, 0);
		drawSphere(0.1);
		glTranslated(0, -0.08, 0);
		setDiffuseColor(0, 0, 0);
		drawSphere(0.05);
		glTranslated(-0.4, 0, 0);
		drawSphere(0.05);

		glPopMatrix();

		/*********************************/


		// face
		glPushMatrix();

			// nose & cheek
			glPushMatrix();
			setDiffuseColor(BODY_COLOR);

			drawCylinder(1.2, 0.2, 0.3);
			glTranslated(0, 0, 1.2);
			drawSphere(0.3);
			glTranslated(1.5, 0, -1.3);
			drawSphere(0.4);
			glTranslated(0.1, 0, 0.4);
			setDiffuseColor(1, 0, 0);

			drawSphere(0.05);
			glTranslated(-0.2, 0, 0);
			 drawSphere(0.05);
			glTranslated(0.1, 0.1, 0);
			drawSphere(0.05);

			setDiffuseColor(BODY_COLOR);
			glTranslated(-3, -0.1, -0.4);
			drawSphere(0.4);
			glTranslated(0.1, 0, 0.4);
			setDiffuseColor(1, 0, 0);

			drawSphere(0.05);
			glTranslated(-0.2, 0, 0);
			drawSphere(0.05);
			glTranslated(0.1, 0.1, 0);
			drawSphere(0.05);
			glPopMatrix();

			// mouth
			glPushMatrix();
			glTranslated(-1.5, 0, 0);
			glRotated(90, 0.0, 1.0, 0.0);
			setDiffuseColor(0, 0, 0);
			if (false) {

			}
			else
			{
				glRotated(45, 1.0, 0.0, 0.0);
				drawCylinder(0.5, 0.05, 0.05);
				glTranslated(0, 0, 0.5);
				glRotated(-15, 1.0, 0.0, 0.0);
				drawCylinder(0.5, 0.05, 0.05);
				glTranslated(0, 0, 0.5);
				glRotated(-15, 1.0, 0.0, 0.0);
				drawCylinder(0.4, 0.05, 0.05);
				glTranslated(0, 0, 0.4);
				glRotated(-15, 1.0, 0.0, 0.0);

				// teeth
				glPushMatrix();
					setDiffuseColor(0.8, 0.8, 0.8);
					glTranslated(0, 0, 0.2);
					glRotated(10, 1, 0, 0.0);
					drawBox(-0.1, -0.4, -0.4);
					glTranslated(0, 0.03, 0.2);
					glRotated(-20, 1, 0, 0.0);
					drawBox(-0.1, -0.4, 0.4);
				glPopMatrix();

				setDiffuseColor(0, 0, 0);
				drawCylinder(0.5, 0.05, 0.05);
				glTranslated(0, 0, 0.5);
				glRotated(-15, 1.0, 0.0, 0.0);
				drawCylinder(0.5, 0.05, 0.05);
				glTranslated(0, 0, 0.5);
				glRotated(-15, 1.0, 0.0, 0.0);
				drawCylinder(0.5, 0.05, 0.05);
				glTranslated(0, 0, 0.5);
				glRotated(-15, 1.0, 0.0, 0.0);
				drawCylinder(0.5, 0.05, 0.05);

			}
			glPopMatrix();

			// eye
			glTranslated(0, 0.5f, 0);
			glPushMatrix();
				glTranslated(-0.75f, 0, -.75f);
				// eyelash
				glPushMatrix();
				setDiffuseColor(0, 0, 0);
				glTranslated(0, 0, 0.75);
				glRotated(-90, 1.0, 0.0, 0.0);

				drawCylinder(1.1, 0.1, 0.1);
				glRotated(30, 0.0, 1.0, 0.0);
				drawCylinder(1.1, 0.1, 0.1);
				glRotated(-60, 0.0, 1.0, 0.0);
				drawCylinder(1.1, 0.1, 0.1);
				glPopMatrix();

				// eyeball
				setDiffuseColor(1, 1, 1);
				drawSphere(1.0f);
				glRotated(VAL(EYE_Y), 0.0, 1.0, 0.0);
				glRotated(VAL(EYE_X), 1.0, 0.0, 0.0);
				glTranslated(0, 0, 0.75);
				setDiffuseColor(.5f, .5f, 1.0f);
				drawSphere(0.35f);
				glTranslated(0, 0, 0.1);
				setDiffuseColor(0, 0, 0);
				drawSphere(0.27f);
			glPopMatrix();
			glPushMatrix();
				glTranslated(0.75f, 0, -.75f);
				// eyelash
				glPushMatrix();
				setDiffuseColor(0, 0, 0);
				glTranslated(0, 0, 0.75);
				glRotated(-90, 1.0, 0.0, 0.0);
				drawCylinder(1.1, 0.1, 0.1);
				glRotated(30, 0.0, 1.0, 0.0);
				drawCylinder(1.1, 0.1, 0.1);
				glRotated(-60, 0.0, 1.0, 0.0);
				drawCylinder(1.1, 0.1, 0.1);
				glPopMatrix();


				// eyeball
				setDiffuseColor(1, 1, 1);
				drawSphere(1.0f);
				glRotated(VAL(EYE_Y), 0.0, 1.0, 0.0);
				glRotated(VAL(EYE_X), 1.0, 0.0, 0.0);
				glTranslated(0, 0, 0.75);
				setDiffuseColor(.5f, .5f, 1.0f);
				drawSphere(0.35f);
				glTranslated(0, 0, 0.1);
				setDiffuseColor(0, 0, 0);
				drawSphere(0.27f);
			glPopMatrix();
		glPopMatrix();

	

		// left arm
		glPushMatrix();
		// upper arm
		glTranslated(2, 0, -1);
		setDiffuseColor(WHITE_COLOR);
		drawSphere(.4f);
		glRotated(lhux, 1.0, 0.0, 0.0);
		glRotated(lhuy, 0.0, 1.0, 0.0);
		glRotated(lhuz, 0.0, 0.0, 1.0);
		setDiffuseColor(WHITE_COLOR);
		drawCylinder(0.5f, .4f, .4f);
		setDiffuseColor(BODY_COLOR);
		drawCylinder(2, .2f, .2f);
		// lower arm
			glPushMatrix();
			glTranslated(0, 0, 2);
			setDiffuseColor(BODY_COLOR);
			drawSphere(.2f);
			glRotated(lhl, 1.0, 0.0, 0.0);
			setDiffuseColor(BODY_COLOR);
			drawCylinder(2, .2f, .2f);
			glPushMatrix();
			glTranslated(0, 0, 2);
			glRotated(-90, 1.0, 0, 0);
			if (false) 
			{

			}
			else if (true) 
			{
				setDiffuseColor(.30f, .15f, .0f);
				drawCylinder(3, 0.1f, 0.1f);
				glTranslated(0, 0, 4);
				drawTorus(1, 0.15f);
				bubbles->setTransformMatrix(cameraMatrix.inverse()* getModelViewMatrix());
			}
			else if (0)
			{
				setDiffuseColor(0.92f, 0.49f, 0.62f);
				for (int i = 0; i < 5; i++)
				{
					double angle = 360.0 / 5 * i;
					double offset = 36.0;
					double L0 = 1.5f;
					double L1 = 0.5f;
					double H = 0.2f;

					double s0 = L0 * sin(angle * M_PI / 180.0);
					double c0 = L0 * cos(angle * M_PI / 180.0);
					double sL = L1 * sin((angle - offset) * M_PI / 180.0);
					double cL = L1 * cos((angle - offset) * M_PI / 180.0);
					double sR = L1 * sin((angle + offset) * M_PI / 180.0);
					double cR = L1 * cos((angle + offset) * M_PI / 180.0);

					drawTriangle(0, 0, -H, c0, s0, 0, cL, sL, 0);
					drawTriangle(0, 0, -H, cR, sR, 0, c0, s0, 0);
					drawTriangle(0, 0, H, cL, sL, 0, c0, s0, 0);
					drawTriangle(0, 0, H, c0, s0, 0, cR, sR, 0);
	

				}
			}

			glPopMatrix();
			glPopMatrix();
		glPopMatrix();

		// right arm
		glPushMatrix();
		// upper arm
		glTranslated(-2, 0, -1);
		setDiffuseColor(WHITE_COLOR);
		drawSphere(.4f);
		glRotated(rhux, 1.0, 0.0, 0.0);
		glRotated(rhuy, 0.0, 1.0, 0.0);
		glRotated(rhuz, 0.0, 0.0, 1.0);
		setDiffuseColor(WHITE_COLOR);
		drawCylinder(0.5f, .4f, .4f);
		setDiffuseColor(BODY_COLOR);
		drawCylinder(2, .2f, .2f);
		// lower arm
			glPushMatrix();
			glTranslated(0, 0, 2);
			setDiffuseColor(BODY_COLOR);
			drawSphere(.2f);
			glRotated(rhl, 1.0, 0.0, 0.0);
			setDiffuseColor(BODY_COLOR);
			drawCylinder(2, .2f, .2f);
		//Hand
			glPushMatrix();
			glTranslated(0, 0, 2);
			glScaled(0.6, 1, 1);
			drawSphere(0.3);
			glScaled(1.67, 1, 1);
			drawCylinder(0.5, 0.1, 0.1);
			glRotated(15, 1, 0, 0);
			drawCylinder(0.5, 0.1, 0.1);
			glRotated(15, 1, 0, 0);
			drawCylinder(0.5, 0.1, 0.1);
			glRotated(-45, 1, 0, 0);
			drawCylinder(0.5, 0.1, 0.1);
			glRotated(-15, 1, 0, 0);
			drawCylinder(0.5, 0.1, 0.1);
			glPopMatrix();
			
			glPopMatrix();
		glPopMatrix();

		// lower body
		glPushMatrix();
		glTranslated(0, -2.5f, 0);

			// suit
			glPushMatrix();

			glTranslated(-2, -0.5f, -2);
			if (true) {
				setDiffuseColor(1, 1, 1);
				drawTextureBox(4.0f, 1.0f, 2.0f, bodyTex);
			}
			else {
				setDiffuseColor(.30f, .15f, .0f);
				drawBox(4.0f, 1.0f, 2.0f);
			}

			glPopMatrix();

			// right leg
			glPushMatrix(); 
			setDiffuseColor(.30f, .15f, .0f);
			glTranslated(-1, -.5f, -1);
			glRotated(VAL(RLEG_UPPER_X), 1.0, 0.0, 0.0);
			glRotated(VAL(RLEG_UPPER_Y), 0.0, 1.0, 0.0);
			glRotated(VAL(RLEG_UPPER_Z), 0.0, 0.0, 1.0);
			drawSphere(.5f);
			drawCylinder(0.5f, .5f, .5f);
			setDiffuseColor(BODY_COLOR);
			drawCylinder(2, .2f, .2f);

				glPushMatrix();
				glTranslated(0, 0, 2);
				glRotated(VAL(RLEG_LOWER), 1.0, 0.0, 0.0);
				setDiffuseColor(BODY_COLOR);
				drawSphere(.2f);
				drawCylinder(.5f, .2f, .2f);
				glTranslated(0, 0, 0.5f);
				setDiffuseColor(WHITE_COLOR);
				drawCylinder(1.5f, .2f, .2f);
				
					// shoes
					glPushMatrix();
					setDiffuseColor(0, 0, 0);
					glTranslated(0, 0.0, 1.5);
					drawSphere(0.5);
					glTranslated(-0.4, -0.4, 0.1);
					drawBox(0.8, 0.8, 0.4);
					glTranslated(0.4, 0.4, -0.1);
					glRotated(-90, 1.0, 0.0, 0.0);
					drawCylinder(1, 0.2, 0.5);
					glTranslated(0, 0.0, 1);
					drawSphere(0.5);
					glPopMatrix();

				glPopMatrix();

			glPopMatrix();

			// left leg
			glPushMatrix();
			setDiffuseColor(.30f, .15f, .0f);
			//if (VAL(SUIT) == 1) setDiffuseColor(.11f, .54f, .82f);
			glTranslated(1, -.5f, -1);
			glRotated(VAL(LLEG_UPPER_X), 1.0, 0.0, 0.0);
			glRotated(VAL(LLEG_UPPER_Y), 0.0, 1.0, 0.0);
			glRotated(VAL(LLEG_UPPER_Z), 0.0, 0.0, 1.0);
			drawSphere(.5f);
			drawCylinder(0.5f, .5f, .5f);
			setDiffuseColor(BODY_COLOR);
			drawCylinder(2, .2f, .2f);

				glPushMatrix();
				glTranslated(0, 0, 2);
				glRotated(VAL(LLEG_LOWER), 1.0, 0.0, 0.0);
				setDiffuseColor(BODY_COLOR);
				drawSphere(.2f);
				drawCylinder(.5f, .2f, .2f);
				glTranslated(0, 0, 0.5f);
				setDiffuseColor(WHITE_COLOR);
				drawCylinder(1.5f, .2f, .2f);
				
					// shoe
					glPushMatrix();
					setDiffuseColor(0, 0, 0);
					glTranslated(0, 0.0, 1.5);
					drawSphere(0.5);
					glTranslated(-0.4, -0.4, 0.1);
					drawBox(0.8, 0.8, 0.4);
					glTranslated(0.4, 0.4, -0.1);
					glRotated(-90, 1.0, 0.0, 0.0);
					drawCylinder(1, 0.2, 0.5);
					glTranslated(0, 0.0, 1);
					drawSphere(0.5);
					glPopMatrix();

				glPopMatrix();

			glPopMatrix();
		
		glPopMatrix();

	glPopMatrix();

	// draw the floor
	/*
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_BLUE);
	glPushMatrix();
	glTranslated(-5, -7.5, -5);
	drawBox(10, 0.01f, 10);
	glPopMatrix();
	*/

	setDiffuseColor(WHITE_COLOR);

	endDraw();

}

Vec3d SpongebobModel::getJointPosition(int i, int j)
{
	Vec3d root = Vec3d(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
	Vec3d limb = Vec3d(0, 0, 2);
	Vec3d x(1, 0, 0), y(0, 1, 0), z(0, 0, 1); // basis
	int para;
	switch (i)
	{
	case LEFT_HAND:
		para = LHAND_UPPER_X;
		root += Vec3d(2, 0, -1);
		break;
	case RIGHT_HAND:
		para = RHAND_UPPER_X;
		root += Vec3d(-2, 0, -1);
		break;
	case LEFT_LEG:
		para = LLEG_UPPER_X;
		root += Vec3d(1, -3, -1);
		break;
	case RIGHT_LEG:
		para = RLEG_UPPER_X;
		root += Vec3d(-1, -3, -1);
		break;

	}

	if (j == 0) return root;
	Mat4d r0x = Mat4d::createRotation(VAL(para), 1, 0, 0);
	x = r0x * x; y = r0x * y; z = r0x * z;
	Mat4d r0y = Mat4d::createRotation(VAL(para+1), y[0], y[1], y[2]);
	x = r0y * x; y = r0y * y; z = r0y * z;
	Mat4d r0z = Mat4d::createRotation(VAL(para+2), z[0], z[1], z[2]);
	x = r0z * x; y = r0z * y; z = r0z * z;
	if (j == 1) return root + r0z * r0y * r0x * limb;
	Mat4d r1 = Mat4d::createRotation(VAL(para+3), x[0], x[1], x[2]);
	if (j == 2) return root + r0z * r0y * r0x * limb + r1 * r0z * r0y * r0x * limb;

	return root;
}

void SpongebobModel::loadTexture()
{
	int width, height;
	GLubyte* bitmap = readBMP("assets/suit.bmp", width, height);
	if (bitmap) {
		glGenTextures(1, &bodyTex);
		glBindTexture(GL_TEXTURE_2D, bodyTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, bitmap);

	}

	for (int i = 0; i < 6; i++)
	{
		int width, height;
		GLubyte* bitmap = readBMP(skybox_path[i], width, height);
		if (bitmap) {
			glGenTextures(1, &_skybox[i]);
			glBindTexture(GL_TEXTURE_2D, _skybox[i]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, bitmap);

		}
	}

	hfTex = readBMP("assets/hf.bmp", hfTex_w, hfTex_h);
	hfColorTex = readBMP("assets/hf_color.bmp", hfTex_w, hfTex_h);

	if (hfTex && hfColorTex)
		initDL();

	loadedTexture = true;
}

void SpongebobModel::skybox()
{

	// Store the current matrix
	glPushMatrix();

	// Reset and transform the matrix.
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,
		m_camera->mPosition[0], m_camera->mPosition[1], m_camera->mPosition[2],
		0, 1, 0);

	// Enable/Disable features
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);


	// Just in case we set all vertices to white.
	glColor4f(1, 1, 1, 1);
	glScaled(100, 100, 100);

	// Render the front quad
	glBindTexture(GL_TEXTURE_2D, _skybox[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, -0.5f);
	glEnd();

	// Render the left quad
	glBindTexture(GL_TEXTURE_2D, _skybox[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, 0.5f);
	glEnd();

	// Render the back quad
	glBindTexture(GL_TEXTURE_2D, _skybox[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);

	glEnd();

	// Render the right quad
	glBindTexture(GL_TEXTURE_2D, _skybox[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
	glEnd();

	// Render the top quad
	glBindTexture(GL_TEXTURE_2D, _skybox[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
	glEnd();

	// Render the bottom quad
	glBindTexture(GL_TEXTURE_2D, _skybox[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 1); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
	glEnd();

	// Restore enable bits and matrix
	glPopAttrib();
	glPopMatrix();

}

void SpongebobModel::initDL()
{
	hfDL = glGenLists(1);
	glNewList(hfDL, GL_COMPILE);
		glTranslated(-(double)hfTex_w/4, 0, -(double)hfTex_h/4);
		glScaled(0.5, 1, 0.5);
		for (int i = 0; i < hfTex_h - 1; i++)
		{
			for (int j = 0; j < hfTex_w - 1; j++)
			{
				//			mesh->addFace(i * w + j + 1, (i + 1) * w + j, i * w + j);
				//			mesh->addFace(i * w + j + 1, (i + 1) * w + j + 1, (i + 1) * w + j);
				double r1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3] / 255.0;
				double g1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3 + 1] / 255.0;
				double b1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3 + 2] / 255.0;

				double r2 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3] / 255.0;
				double g2 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3 + 1] / 255.0;
				double b2 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3 + 2] / 255.0;

				double r3 = (double)hfColorTex[(i * hfTex_w + j) * 3] / 255.0;
				double g3 = (double)hfColorTex[(i * hfTex_w + j) * 3 + 1] / 255.0;
				double b3 = (double)hfColorTex[(i * hfTex_w + j) * 3 + 2] / 255.0;

				double r = (r1 + r2 + r3) / 3;
				double g = (g1 + g2 + g3) / 3;
				double b = (b1 + b2 + b3) / 3;

				double h1 = (double)hfTex[(i * hfTex_w + j + 1) * 3] / 255.0;
				double h2 = (double)hfTex[((i+1) * hfTex_w + j) * 3] / 255.0;
				double h3 = (double)hfTex[(i * hfTex_w + j) * 3] / 255.0;

				setDiffuseColor(r, g, b);
				drawTriangle(i, h1, j + 1, i + 1, h2, j, i, h3, j);

				r1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3] / 255.0;
				g1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3 + 1] / 255.0;
				b1 = (double)hfColorTex[(i * hfTex_w + j + 1) * 3 + 2] / 255.0;

				r2 = (double)hfColorTex[((i + 1) * hfTex_w + j + 1) * 3] / 255.0;
				g2 = (double)hfColorTex[((i + 1) * hfTex_w + j + 1) * 3 + 1] / 255.0;
				b2 = (double)hfColorTex[((i + 1) * hfTex_w + j + 1) * 3 + 2] / 255.0;

				r3 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3] / 255.0;
				g3 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3 + 1] / 255.0;
				b3 = (double)hfColorTex[((i + 1) * hfTex_w + j) * 3 + 2] / 255.0;

				r = (r1 + r2 + r3) / 3;
				g = (g1 + g2 + g3) / 3;
				b = (b1 + b2 + b3) / 3;

				h1 = (double)hfTex[(i * hfTex_w + j + 1) * 3] / 255.0;
				h2 = (double)hfTex[((i + 1) * hfTex_w + j + 1) * 3] / 255.0;
				h3 = (double)hfTex[((i + 1) * hfTex_w + j) * 3] / 255.0;

				setDiffuseColor(r, g, b);
				drawTriangle(i, h1, j + 1, i + 1, h2, j + 1, i + 1, h3, j);

			}
		}
	glEndList();
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);

	controls[LIGHT_X] = ModelerControl("Light X Position", -5, 5, 0.1f, 0);
	controls[LIGHT_Y] = ModelerControl("Light Y Position", 5, 10, 0.1f, 5);
	controls[LIGHT_Z] = ModelerControl("Light Z Position", -5, 5, 0.1f, 0);

	controls[HEIGHT] = ModelerControl("Look: Height", 4, 6, 0.1, 4);

	controls[EYE_X] = ModelerControl("Rotate: Eye X", -30, 30, 1, 0);
	controls[EYE_Y] = ModelerControl("Rotate: Eye Y", -30, 30, 1, 0);

	controls[LHAND_UPPER_X] = ModelerControl("Rotate: Left Arm X", -180, 180, 1, 90);
	controls[LHAND_UPPER_Y] = ModelerControl("Rotate: Left Arm Y", 0, 135, 1, 0);
	controls[LHAND_UPPER_Z] = ModelerControl("Rotate: Left Arm Z", -90, 180, 1, 0);
	controls[LHAND_LOWER] = ModelerControl("Rotate: Left Elbow", -160, 0, 1, 0);
	controls[RHAND_UPPER_X] = ModelerControl("Rotate: Right Arm X", -135, 135, 1, 90);
	controls[RHAND_UPPER_Y] = ModelerControl("Rotate: Right Arm Y", -135, 135, 1, 0);
	controls[RHAND_UPPER_Z] = ModelerControl("Rotate: Right Arm Z", -180, 90, 1, 0);
	controls[RHAND_LOWER] = ModelerControl("Rotate: Right Elbow", -160, 0, 1, 0);
	controls[LLEG_UPPER_X] = ModelerControl("Rotate: Left Leg X", -135, 135, 1, 90);
	controls[LLEG_UPPER_Y] = ModelerControl("Rotate: Left Leg Y", -135, 135, 1, 0);
	controls[LLEG_UPPER_Z] = ModelerControl("Rotate: Left Leg Z", -135, 135, 1, 0);
	controls[LLEG_LOWER] = ModelerControl("Rotate: Left Knee", 0, 120, 1, 0);
	controls[RLEG_UPPER_X] = ModelerControl("Rotate: Right Leg X", -135, 135, 1, 90);
	controls[RLEG_UPPER_Y] = ModelerControl("Rotate: Right Leg Y", -135, 135, 1, 0);
	controls[RLEG_UPPER_Z] = ModelerControl("Rotate: Right Leg Z", -135, 135, 1, 0);
	controls[RLEG_LOWER] = ModelerControl("Rotate: Right Knee", 0, 120, 1, 0);

	controls[IK_LIMB] = ModelerControl("IK: Select Limb", 0, 4, 1, 0);
	controls[IK_TARGET_X] = ModelerControl("IK: Target (Jellyfish) X", -20, 20, 0.1, -3);
	controls[IK_TARGET_Y] = ModelerControl("IK: Target (Jellyfish) Y", -20, 20, 0.1, 4);
	controls[IK_TARGET_Z] = ModelerControl("IK: Target (Jellyfish) Z", -20, 20, 0.1, 1);
	controls[IK_MIN] = ModelerControl("IK: Min Elbow Angle", 0, 180, 1, 0);
	controls[IK_MAX] = ModelerControl("IK: Max Elbow Angle", 0, 180, 1, 180);

	controls[JELLY_MOVE] = ModelerControl("Jellyfish Move", -30, 30, 0.1, 0);
	controls[SNAIL_MOVE] = ModelerControl("Snail Move", -2, 0, 0.1, 0);

	srand(time(0));
	ParticleSystem* ps = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(ps);

	ModelerApplication::Instance()->Init(&createSpongebobModel, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}

