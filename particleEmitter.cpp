#include "particleEmitter.h"

void ParticleEmitter::emit(vector<Particle>& p, int count)
{
	Particle particle;
	Vec3d coord(0, 0, 0);
	particle.x = tx * coord;
	particle.m = 1;

	p.push_back(particle);
	
}