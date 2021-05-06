#ifndef _IK_
#define _IK_

#include "vec.h"
#include "mat.h"
#include "Spongebob.h"

enum {
	LEFT_HAND,
	RIGHT_HAND,
	LEFT_LEG,
	RIGHT_LEG
};

class IK {
public:
	static Vec3d joints[3];
	static double angles[4];
	static int numJoint;
	static double* d;
	static Vec3d target;
	static int limb;
	static double minA, maxA;

	static double tolerence;
	static int maxIterations;

public:
	IK();
	static void setLimb(int i);
	static void setTarget(Vec3d t);
	static void setConstraints(double minA, double maxA);
	static void solve();
	static void applyConstraints();
	static void PosToRot();
	static void RotToPos();
	static void setRotations();
	static void apply(double& ux, double& uy, double& uz, double& l);


};

#endif // !_IK_
