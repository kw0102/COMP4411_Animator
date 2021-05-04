#ifndef __PARTICLE_
#define __PARTICLE_

#include "vec.h"

struct Particle
{
	Vec3d x;
	Vec3d v;
	Vec3d f;
	double m;

};

enum Forces {
	GRAVITY,
	WIND,
	FLOCKING
};

#endif // !__PARTICLE_
