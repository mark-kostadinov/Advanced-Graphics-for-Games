#include "stdafx.h"
#include "RainEmitter.h"

RainEmitter::RainEmitter()
{
	SetParticleRate(70.0f);
	SetParticleLifetime(20000.0f);
	SetParticleSize(120.0f);
	SetParticleVariance(0.01f);
	SetParticleSpeed(4.0f);
	SetLaunchParticles(64);

	// Each rain particle is a white stroke, which has an alpha fade on it, so the edges fade to 0.0 alpha.
	texture = SOIL_load_OGL_texture(TEXTUREDIR"rain_particle.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
}

RainEmitter::~RainEmitter()
{
}

void RainEmitter::Update(float msec, HeightMap* heightMap)
{
	nextParticleTime -= msec;

	while (nextParticleTime <= 0)
	{
		nextParticleTime += particleRate;
		
		for (int i = 0; i < numLaunchParticles; ++i)
			particles.push_back(GetFreeParticle());
	}

	for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end();)
	{
		Particle *p = (*it);
		p->colour.w -= (msec / particleLifetime);

		// 1. If this particle has ran out of life;
		// 2. If a Collision is detected (i.e. an active particle’s y coordinate is
		// the same as that of the particular part of the terrain it passed through);
		// 3. If the particle goes out of the heightMap boundaries
		// => Retire the particle and send it to the ‘Free list’ for future use
		if (p->colour.w <= 0.0f || IsParticleColliding(this, p, heightMap) || IsParticleOutOfBounds(this, p))
		{
			freeList.push_back(p);
			it = particles.erase(it);
		}
		else
		{
			p->position += p->direction * (msec * particleSpeed);
			++it;
		}
	}

	if (particles.size() > largestSize)
		ResizeArrays();
}

bool RainEmitter::IsParticleColliding(RainEmitter * r, Particle * p, HeightMap * heightMap) const
{
	// Get the relative position of each particle and combine it with the world position of the emitter 
	// to get the world position of the particle, which is needed in the GetTerrainHeight() function
	float particleWorldX = emitterPosition.x + p->position.x;
	float particleWorldY = emitterPosition.y + p->position.y;
	float particleWorldZ = emitterPosition.z + p->position.z;

	// Get the height on the particle's coordinates
	float terrainHeight = heightMap->GetTerrainHeight(particleWorldX, particleWorldZ);

	// Check if there's a collision
	if (particleWorldY <= terrainHeight)
	{
		// Get the colliding particle's world position to send to the snow collision shader as uniform data
		r->SetFallenRaindropPosX(particleWorldX);
		r->SetFallenRaindropPosZ(particleWorldZ);

		return true;
	}
	// Catch if something abnormal happens
	if (particleWorldY <= 0)
	{
		return true;
	}
	return false;
}

bool RainEmitter::IsParticleOutOfBounds(RainEmitter * s, Particle * p) const
{
	// Get the relative position of each particle and combine it with the world position of the emitter 
	// to get the world position of the particle, which is needed in the evaluation afterwards
	float particleWorldX = emitterPosition.x + p->position.x;
	float particleWorldZ = emitterPosition.z + p->position.z;

	// Check if the particle is out of bounds
	if (particleWorldX >= RAW_HEIGHT * HEIGHTMAP_X || particleWorldZ >= RAW_WIDTH * HEIGHTMAP_Z || particleWorldX <= 0.0f || particleWorldZ <= 0.0f)
	{
		return true;
	}
	return false;
}

Particle* RainEmitter::GetFreeParticle()
{
	Particle * p = NULL;

	// If we have a spare particle on the freelist, pop it off!
	if (freeList.size() > 0)
	{
		p = freeList.back();
		freeList.pop_back();
	}
	else
	{
		// No spare particles, so we need a new one
		p = new Particle();
	}
	// Give the particle a starting position within a fixed range
	p->position.x = (float)(rand() % (RAW_HEIGHT * RAW_WIDTH) + (-(emitterPosition.x)));
	p->position.y = emitterPosition.y;
	p->position.z = (float)(rand() % (RAW_HEIGHT * RAW_WIDTH) + (-(emitterPosition.z)));

	p->colour = RAINDROP_COLOUR;
	p->direction = initialDirection;
	p->direction.x = 0;
	p->direction.y += -(fabs((RAND() - RAND()) * particleVariance)); // Return the absolute value, then convert to negative
	// Don't let them freeze in the sky
	if (p->direction.y > -0.01f)
	{
		p->direction.y = -0.01f;
	}
	p->direction.z = 0;
	p->direction.Normalise();

	return p;
}

