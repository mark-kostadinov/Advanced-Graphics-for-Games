#pragma once
#include "Vector4.h"
#include "Vector3.h"

class Light
{
public:
	Light() {}
	Light(Vector3 position, Vector4 colour, float radius)
	{
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

	// Addition by Mark Kostadinov <m.kostadinov1@newcastle.ac.uk> 
	float GetMovementCounter() const { return movementCounter; }
	void SetMovementCounter(float f) { movementCounter = f; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;
	float movementCounter = 0.0f;
};
