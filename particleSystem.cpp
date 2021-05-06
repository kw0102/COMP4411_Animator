#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	// TODO
	bake_fps = 50;

	simulate = false;
	dirty = false;

}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO
	for (auto e : emitters)
	{
		delete e;
	}
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	// TODO

	if (!simulate)
	{
		bake_start_time = t;
	}

	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    
	// TODO

	bake_end_time = t;

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	// TODO
	for (auto emitter : emitters)
	{
		emitter->numParticles = 0;
	}

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{

	// TODO

	// baked
	if (cache.find(getFrame(t)) != cache.end() || !simulate)
		return;

	float dt = 1.0f / bake_fps;
	int totalFrame = getFrame(t);
	for (int frame = 0; frame <= totalFrame; frame++)
	{
		// dont rebake
		if (cache.find(frame) != cache.end()) continue;

		// handle initial frame
		if (frame == 0)
		{
			particles.clear();
			for (auto emitter : emitters)
			{
				emitter->emit(particles);
			}
			//bakeParticles(_t);
			cache[frame] = particles;

			continue;
		}

		particles = cache[frame - 1];

		// Clear dead particles
		for (auto it = particles.begin(); it != particles.end();)
		{
			it->life -= dt;
			if (it->life < 0) {
				it->src->numParticles--;
				it = particles.erase(it);
			}
			else
				++it;
		}

		// Clear force accumulator
		for (Particle& p : particles)
			p.f = Vec3d(0, 0, 0);

		// Calculate force
		for (Particle& p : particles)
		{
			if (p.f_types & GRAVITY)
			{
				p.f += Vec3d(0, -0.5, 0);
			}
			if (p.f_types & WIND)
			{
				p.f += Vec3d(-0.2 * frand(0.5, 2), 0, -0.1 * frand(0.5, 2));
			}
			if (p.f_types & FLOCKING)
			{
				double preceptionRadius = 2.0f;
				double maxForce = 2.0f;
				double speed = 5.0f;

				Vec3d alignment, cohesion, separation;
				int numNeighbor = 0;

				for (Particle& other : particles)
				{
					if (&other == &p) continue;
					Vec3d distanceVector = p.x - other.x;
					if (distanceVector.length2() < preceptionRadius * preceptionRadius)
					{
						alignment += other.v;
						cohesion += other.x;
						separation += (p.x - other.x) / distanceVector.length();
						numNeighbor++;
					}
				}

				if (numNeighbor > 0)
				{
					alignment = alignment / numNeighbor;
					if (alignment.length() < speed)
					{
						alignment.normalize();
						alignment *= speed;
					}
					cohesion = cohesion / numNeighbor;
					separation = separation / numNeighbor;
					Vec3d alignmentForce = alignment - p.v;
					Vec3d cohesionForce = cohesion - p.x - p.v;
					Vec3d separationForce = separation - p.v;

					if (alignmentForce.length2() > maxForce * maxForce)
					{
						alignmentForce.normalize();
						alignmentForce *= maxForce;
					}

					if (cohesionForce.length2() > maxForce * maxForce)
					{
						cohesionForce.normalize();
						cohesionForce *= maxForce;
					}

					if (separationForce.length2() > maxForce * maxForce)
					{
						separationForce.normalize();
						separationForce *= maxForce;
					}

					p.f += 1 * alignmentForce + 0.375 * cohesionForce + 0.5 * separationForce;

				}
			}
				
		}

		// Euler's method
		for (Particle& p : particles)
		{
			p.x += p.v * dt;
			p.v += p.f * (1 / p.m) * dt;
			if (p.f_types & FLOCKING)
			{
				for (int i = 0; i < 3; i++)
				{
					if (p.x[i] < -15) p.x[i] += 30;
					if (p.x[i] > 15) p.x[i] -= 30;
				}

			}
		}

		// Emit particles
		for (auto emitter : emitters)
		{
			emitter->emit(particles);
		}
		

		//bakeParticles(_t);
		cache[frame] = particles;

	}

}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{

	// TODO
	int frame = getFrame(t);
	auto it = cache.find(frame);

	if (it == cache.end()) return;

	for (Particle& p : it->second)
	{

		setDiffuseColor(p.color[0], p.color[1], p.color[2]);
		glPushMatrix();
		glTranslated(p.x[0], p.x[1], p.x[2]);
		if (p.f_types & FLOCKING)
		{	
			Vec3d dir = p.v;
			dir.normalize();
			Vec3d z = Vec3d(0, 0, 1);
			Vec3d a = cross(z, dir);
			double omega = acos(z * dir) * 180 / M_PI;
			glRotated(omega, a[0], a[1], a[2]);
			glPushMatrix();
			glScaled(0.5, 1, 1);
			drawCylinder(0.5, 0.3, 0.1);
			glPopMatrix();
			glTranslated(0, 0, 0.5);
			glRotated(90, 0, 1, 0);
			glScaled(1, 0.5, 1);
			glTranslated(0, 0, -0.075);
			drawCylinder(0.15, 0.4, 0.4);
		}
		else
		{
			drawSphere(0.2);
		}
		glPopMatrix();
	}


}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{

	// TODO
	cache[getFrame(t)] = particles;


}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{

	// TODO
	cache.clear();
	for (auto emitter : emitters)
	{
		emitter->numParticles = 0;
	}

}





