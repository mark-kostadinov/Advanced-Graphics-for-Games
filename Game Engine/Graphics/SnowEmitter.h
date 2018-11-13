#pragma once

#include "../../nclgl/ParticleEmitter.h"

class SnowEmitter : public ParticleEmitter
{
public:
	SnowEmitter();
	~SnowEmitter();

	void Update(float msec, HeightMap* heightMap);

	bool IsParticleColliding(SnowEmitter* s, Particle* p, HeightMap* heightMap) const;
	bool IsParticleOutOfBounds(SnowEmitter* s, Particle* p) const;

	Particle* GetFreeParticle();

	float GetFallenSnowflakePosX() const { return fallenSnowflakePosX; }
	void SetFallenSnowflakePosX(float p) { fallenSnowflakePosX = p; }
	float GetFallenSnowflakePosZ() const { return fallenSnowflakePosZ; }
	void SetFallenSnowflakePosZ(float p) { fallenSnowflakePosZ = p; }

protected:
	float fallenSnowflakePosX;
	float fallenSnowflakePosZ;
};