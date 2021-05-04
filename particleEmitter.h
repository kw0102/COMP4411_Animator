#ifndef __PARTICLE_EMITTER
#define __PARTICLE_EMITTER

#include <vector>

#include "mat.h"
#include "particle.h"

#include <random>
#include <time.h>

class ParticleEmitter
{
public:
	ParticleEmitter()
	{
		srand(time(0));
	}
	void emit(vector<Particle>& p, int count);
	void setTransformMatrix(Mat4d mat) { tx = mat; };

	Mat4d tx;


	double frand(double min, double max) { return min + ((double)rand() / RAND_MAX) * (max - min); }
};

#endif // !__PARTICLE_EMITTER
