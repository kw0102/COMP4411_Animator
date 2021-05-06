#ifndef _SPONGEBOB_
#define _SPONGEBOB_

#include "modelerview.h"
#include "vec.h"
#include <FL/gl.h>
#include "Metaball.h"
#include "particleEmitter.h"
#include "particleSystem.h"
#include "modelerapp.h"

// To make a SampleModel, we inherit off of ModelerView
class SpongebobModel : public ModelerView
{
public:
	SpongebobModel(int x, int y, int w, int h, char* label)
		: ModelerView(x, y, w, h, label) { 
		snailShell.addBall(Vec3d(0, 2, 0), 1);
		snailShell.addBall(Vec3d(-1.7, 1, 0), 1);
		snailShell.addBall(Vec3d(0, -2, 0), 1);
		snailShell.addBall(Vec3d(-1.7, -1, 0), 1);
		snailShell.addBall(Vec3d(1.7, 1, 0), 1);
		snailShell.addBall(Vec3d(1.7, -1, 0), 1);
		snailShell.addBall(Vec3d(0, 0, 0), 0.8);

		fishes = new ParticleEmitter(FLOCKING, 50, 50);
		ModelerApplication::Instance()->GetParticleSystem()->addEmitter(fishes);

		bubbles = new ParticleEmitter(GRAVITY | WIND, 1, 1000);
		ModelerApplication::Instance()->GetParticleSystem()->addEmitter(bubbles);



	}

	virtual void draw();

	bool loadedTexture = false;
	void loadTexture();

	GLuint bodyTex;
	GLuint _skybox[6];
	char skybox_path[6][50] = { "assets/uw_lf.bmp" , "assets/uw_ft.bmp" , "assets/uw_rt.bmp", "assets/uw_bk.bmp" , "assets/uw_up.bmp" , "assets/uw_dn.bmp" };
	void skybox();
	GLubyte* hfTex = 0;
	GLubyte* hfColorTex = 0;
	int hfTex_w, hfTex_h;

	GLuint hfDL = 0;
	void initDL();
	
	ParticleEmitter* bubbles;
	ParticleEmitter* fishes;

	Metaball snailShell;

	unsigned int frameCount = 0;

	static Vec3d getJointPosition(int i, int j);
};

#endif // !_SPONGEBOB_
