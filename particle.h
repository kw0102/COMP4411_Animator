#ifndef __PARTICLE_
#define __PARTICLE_

#include "vec.h"

class ParticleEmitter;

struct Particle
{
	Vec3d x;
	Vec3d v;
	Vec3d f;
	double m;
	int f_types;
	ParticleEmitter* src;
	Vec3d color;
	double life;
};

enum Force {
	GRAVITY = 1,
	WIND = 2,
	FLOCKING = 4
};

#endif // !__PARTICLE_
