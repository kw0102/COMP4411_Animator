/*
*  FABRIK Inverse Kinematics
* 
*  Based on https://www.researchgate.net/publication/220632147_FABRIK_A_fast_iterative_solver_for_the_Inverse_Kinematics_problem
*/

#include "IK.h"
#include "modelerglobals.h"
#include "modelerapp.h"
#include <math.h>

#define clamp(x, a, b) ((x > b) ? b : (x < a) ? a : x)

Vec3d IK::joints[3];
int IK::numJoint = 3;
double IK::angles[4];
double* IK::d = new double[3];
Vec3d IK::target = Vec3d(0,0,0);
int IK::limb = 0;
double IK::minA = 0.0f;
double IK::maxA = 180.0f;


double IK::tolerence = 0.01f;
int IK::maxIterations = 10;


IK::IK() {};

void IK::setLimb(int n) {
	
	limb = n;
	for (int i = 0; i < 3; i++)
	{
		joints[i] = SpongebobModel::getJointPosition(n, i);
		if (i > 0)
			d[i - 1] = (joints[i] - joints[i - 1]).length();
	}
};

void IK::setTarget(Vec3d t) {
	target = t;
};

void IK::setConstraints(double minA, double maxA)
{
	IK::minA = minA;
	IK::maxA = maxA;
}

void IK::solve() {

	// Find distance from root to target
	Vec3d r2t = target - joints[0];
	double targetDist = r2t.length();

	// Find max reach
	double totalDist = 0.0f;
	for (int i = 0; i < numJoint - 1; i++) {
		totalDist += d[i];
	}

	if (targetDist > totalDist && 0) {
		// unreachable
		for (int i = 0; i < numJoint - 1; i++) {
			Vec3d r = target - joints[i];
			double lambda = d[i] / r.length();
			joints[i + 1] = (1 - lambda) * joints[i] + lambda * target;
		}
		PosToRot();
	}
	else
	{
		// reachable
		Vec3d b = joints[0];
		double diff = (joints[numJoint - 1] - target).length();
		int iteration = 0;
		while (diff > tolerence && iteration < maxIterations)
		{
			// forward reaching
			joints[numJoint - 1] = target;
			for (int i = numJoint - 2; i >= 0; i--)
			{
				Vec3d r = joints[i + 1] - joints[i];
				double lambda = d[i] / r.length();
				joints[i] = (1 - lambda) * joints[i + 1] + lambda * joints[i];
			}
			
			PosToRot();
			applyConstraints();
			RotToPos();

			// backward reaching	
			joints[0] = b;
			for (int i = 0; i < numJoint - 1; i++)
			{
				Vec3d r = joints[i + 1] - joints[i];
				double lambda = d[i] / r.length();
				joints[i + 1] = (1 - lambda) * joints[i] + lambda * joints[i + 1];
			}

			PosToRot();
			diff = (joints[numJoint - 1] - target).length();
			iteration++;

		}
	}

	setRotations();
}

void IK::applyConstraints()
{

	if (limb == LEFT_LEG || limb == RIGHT_LEG) return;

	if (minA < maxA && (angles[3] < -maxA || angles[3] > -minA)) {
		double tA = clamp(angles[3], -maxA, -minA);
		double dA = tA - angles[3];

		Vec3d x(1, 0, 0), y(0, 1, 0), z(0, 0, 1); // basis
		Vec3d limb(0, 0, 2);
		Mat4d Rx = Mat4d::createRotation(angles[0], 1, 0, 0);
		x = Rx * x; y = Rx * y; z = Rx * z;
		Mat4d Ry = Mat4d::createRotation(angles[1], y[0], y[1], y[2]);
		x = Ry * x; y = Ry * y; z = Ry * z;
		Mat4d Rz = Mat4d::createRotation(angles[2], z[0], z[1], z[2]);
		x = Rz * x; y = Rz * y; z = Rz * z;
		Mat4d R1 = Mat4d::createRotation(angles[3], x[0], x[1], x[2]);
		x = R1 * x; y = R1 * y; z = R1 * z;

		Vec3d dv = joints[0] - joints[1];
		Mat4d R = Mat4d::createRotation(dA, x[0], x[1], x[2]);
		dv = R * dv;
		joints[0] = joints[1] + dv;

	}
}

void IK::RotToPos()
{
	Vec3d x(1, 0, 0), y(0, 1, 0), z(0, 0, 1); // basis
	Vec3d limb(0, 0, 2);
	Mat4d Rx = Mat4d::createRotation(angles[0], 1, 0, 0);
	x = Rx * x; y = Rx * y; z = Rx * z;
	Mat4d Ry = Mat4d::createRotation(angles[1], y[0], y[1], y[2]);
	x = Ry * x; y = Ry * y; z = Ry * z;
	Mat4d Rz = Mat4d::createRotation(angles[2], z[0], z[1], z[2]);
	x = Rz * x; y = Rz * y; z = Rz * z;
	Mat4d R1 = Mat4d::createRotation(angles[3], x[0], x[1], x[2]);
	joints[1] = joints[0] + Rz * Ry * Rx * limb;
	joints[2] = joints[0] + Rz * Ry * Rx * limb + R1 * Rz * Ry * Rx * limb;
}

void IK::PosToRot()
{
	Vec3d x(1, 0, 0), y(0, 1, 0), z(0, 0, 1); // basis

	Vec3d v0 = joints[1] - joints[0];
	Vec3d v1 = joints[2] - joints[1];

	double proj_x, proj_y, proj_z;
	// find x rotation
	proj_z = (v0 * z);
	proj_y = (v0 * y);
	double rx = -atan2(proj_y, proj_z) * 180.0 / M_PI;
	Mat4d Rx = Mat4d::createRotation(rx, 1, 0, 0);
	x = Rx * x; y = Rx * y; z = Rx * z;


	proj_x = v0 * x;
	proj_z = v0 * z;
	double ry = atan2(proj_x, proj_z) * 180.0 / M_PI;
	Mat4d Ry = Mat4d::createRotation(ry, y[0], y[1], y[2]);
	x = Ry * x; y = Ry * y; z = Ry * z;

	proj_x = v1 * x;
	proj_y = v1 * y;
	double rz = -atan2(proj_x, proj_y) * 180.0 / M_PI;
	Mat4d Rz = Mat4d::createRotation(rz, z[0], z[1], z[2]);
	x = Rz * x; y = Rz * y; z = Rz * z;

	proj_y = v1 * y;
	proj_z = v1 * z;
	double r1 = -atan2(proj_y, proj_z) * 180.0 / M_PI;

	angles[0] = rx; angles[1] = ry; angles[2] = rz; angles[3] = r1;

	if (limb == LEFT_HAND || limb == RIGHT_HAND)
	{
		if (angles[3] > 0.0001f) {
			angles[2] += 180.0;
			angles[3] *= -1;
		}
	}
	else 
	{
		if (angles[3] > -0.0001f) {
			angles[2] += 180.0;
			angles[3] *= -1;
		}
	}
}

void IK::setRotations()
{
	switch (limb)
	{
	case LEFT_HAND:
		SET(LHAND_UPPER_X, angles[0]);
		SET(LHAND_UPPER_Y, angles[1]);
		SET(LHAND_UPPER_Z, angles[2]);
		SET(LHAND_LOWER, angles[3]);
		break;
	case RIGHT_HAND:
		SET(RHAND_UPPER_X, angles[0]);
		SET(RHAND_UPPER_Y, angles[1]);
		SET(RHAND_UPPER_Z, angles[2]);
		SET(RHAND_LOWER, angles[3]);
		break;
	case LEFT_LEG:
		SET(LLEG_UPPER_X, angles[0]);
		SET(LLEG_UPPER_Y, angles[1]);
		SET(LLEG_UPPER_Z, angles[2]);
		SET(LLEG_LOWER, angles[3]);
		break;
	case RIGHT_LEG:
		SET(RLEG_UPPER_X, angles[0]);
		SET(RLEG_UPPER_Y, angles[1]);
		SET(RLEG_UPPER_Z, angles[2]);
		SET(RLEG_LOWER, angles[3]);
		break;
	}
}

void IK::apply(double& ux, double& uy, double& uz, double& l)
{
	ux = angles[0];
	uy = angles[1];
	uz = angles[2];
	l = angles[3];
}

