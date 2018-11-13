#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <random>
#include "../nclgl/Mesh.h"

#define RAW_WIDTH	513
#define RAW_HEIGHT	513
#define RAW_SIZE	512

#define HEIGHTMAP_X 32.0f
#define HEIGHTMAP_Z 32.0f
#define HEIGHTMAP_Y 2.5f
#define HEIGHTMAP_TEX_X 1.0f / 64.0f
#define HEIGHTMAP_TEX_Z 1.0f / 64.0f

#define GRID_SIZE (sizeof(heightGrid) / sizeof(*heightGrid) - 1)

class HeightMap : public Mesh
{
public:
	HeightMap(std::string name);
	~HeightMap(void) {};

	float BarycentricInterpolation(const Vector3 v1, const Vector3 v2, const Vector3 v3, const Vector2 p) const;
	float GetAreaOfTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3) const;
	float GetTerrainHeight(float particleWorldX, float particleWorldZ) const;

private:
	float heightGrid[RAW_HEIGHT][RAW_WIDTH];
};
