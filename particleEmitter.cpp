#include "particleEmitter.h"
#include <math.h>

void ParticleEmitter::emit(vector<Particle>& p)
{
	for (int i = 0; i < numEmit; i++)
	{
		if (numParticles >= maxNumParticles) return;

		Particle particle;
		if ((f_types & FLOCKING))
		{
			Vec3d coord(frand(-5, 5), frand(-5, 5), frand(-5, 5));
			particle.x = tx * coord;
			double theta = frand(0, 2 * M_PI);
			double phi = frand(0, M_PI);
			double mag = frand(2, 4);
			particle.v = Vec3d(sin(phi) * sin(theta), sin(phi) * cos(theta), cos(phi)) * mag;
			particle.m = frand(0.5, 1.5);
			particle.f_types = f_types;
			particle.src = this;
			particle.color = Vec3d(255, 51, 0) / 255;
			particle.color *= frand(0.5, 2.0);
			particle.life = 999;
		}
		else
		{
			Vec3d coord(frand(-0.2, 0.2), frand(-0.2, 0.2), frand(-0.2, 0.2));
			particle.x = tx * coord;
			particle.v = Vec3d(frand(-0.2, 0.2), frand(-0.2, 0.2), frand(-0.2, 0.2));
			particle.m = frand(0.5, 1.5);
			particle.f_types = f_types;
			particle.src = this;
			particle.color = Vec3d(1, 1, 1);
			particle.life = 2.0f;
		}

		p.push_back(particle);
		numParticles++;
	}
}