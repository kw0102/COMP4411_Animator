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
	numEmit = 10;
	maxNumParticles = 1000;

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
		clearBaked();
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
				emitter->emit(particles, numEmit);
			}
			//bakeParticles(_t);
			cache[frame] = particles;

			continue;
		}

		particles = cache[frame - 1];

		// Clear force accumulator
		for (Particle& p : particles)
			p.f = Vec3d(0, 0, 0);

		// Calculate force
		for (Particle& p : particles)
		{
			// Gravity
			p.f += Vec3d(0, -0.1, 0);
			// Wind
			p.f += Vec3d(frand(-0.1, 0.1), 0, frand(-0.1, 0.1));
		}

		// Euler's method
		for (Particle& p : particles)
		{
			p.x += p.v * dt;
			p.v += p.f * (1 / p.m) * dt;
		}

		// Emit particles
		if (particles.size() < maxNumParticles)
		{
			for (auto emitter : emitters)
			{
				emitter->emit(particles, numEmit);
			}
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
		glPushMatrix();
		glTranslated(p.x[0], p.x[1], p.x[2]);
		drawSphere(0.1);
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

}





