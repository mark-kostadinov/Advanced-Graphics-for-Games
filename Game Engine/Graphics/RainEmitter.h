#pragma once

#include "../../nclgl/ParticleEmitter.h"

class RainEmitter : public ParticleEmitter
{
public:
	RainEmitter();
	~RainEmitter();

	void Update(float msec, HeightMap* heightMap);

	bool IsParticleColliding(RainEmitter* s, Particle* p, HeightMap* heightMap) const;
	bool IsParticleOutOfBounds(RainEmitter* s, Particle* p) const;

	Particle* GetFreeParticle();

	float GetFallenRaindropPosX() const { return fallenRaindropPosX; }
	void SetFallenRaindropPosX(float p) { fallenRaindropPosX = p; }
	float GetFallenRaindropPosZ() const { return fallenRaindropPosZ; }
	void SetFallenRaindropPosZ(float p) { fallenRaindropPosZ = p; }

protected:
	float fallenRaindropPosX;
	float fallenRaindropPosZ;
};
