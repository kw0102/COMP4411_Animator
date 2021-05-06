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
	ParticleEmitter(int f_types = 1, int numEmit = 1, int maxNumParticles = 1000) 
		: f_types(f_types), numEmit(numEmit), maxNumParticles(maxNumParticles), numParticles(0) {}
	void emit(vector<Particle>& p);
	void setTransformMatrix(Mat4d mat) { tx = mat; };

	Mat4d tx;

	int f_types;
	int numEmit;
	int maxNumParticles;
	int numParticles;

	double frand(double min, double max) { return min + ((double)rand() / RAND_MAX) * (max - min); }
};

#endif // !__PARTICLE_EMITTER
