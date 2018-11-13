#include "HeightMap.h"

HeightMap::HeightMap(std::string name)
{
	std::ifstream file(name.c_str(), ios::binary);
	if (!file)
	{
		return;
	}
	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	unsigned char * data = new unsigned char[numVertices];
	file.read((char *)data, numVertices * sizeof(unsigned char));
	file.close();

	for (int x = 0; x < RAW_WIDTH; ++x)
	{
		for (int z = 0; z < RAW_HEIGHT; ++z)
		{
			int offset = (x * RAW_WIDTH) + z;
			vertices[offset] = Vector3(x * HEIGHTMAP_X, data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			// Store the height for each vertex
			heightGrid[x][z] = vertices[offset].y;
		}
	}
	delete data;

	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x)
	{
		for (int z = 0; z < RAW_HEIGHT - 1; ++z)
		{
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}
	GenerateNormals();
	GenerateTangents();
	BufferData();
}

// 1. First, some definitions: 
// t - the whole triangle
// t1, t2, t3 - sub-triangles of 2 of the vertices (v1, v2, v3) and an arbitrary position inside t (p)
//
// 2. Find the three barycentric coordinates of a triangle 
// (usually known as Alpha, Beta, and Gamma) as follows:
// A = (Area of t1 / Area of t) 
// B = (Area of t2 / Area of t) 
// G = (Area of t3 / Area of t) or (1.0f - alpha - beta)
//
// 3. We can then determine the interpolated value of any vertex attribute,
// using these barycentric coordinates in the following way:
// barycentre = (v1 * A) + (v2 * B) + (v3 * G)

float HeightMap::BarycentricInterpolation(Vector3 v1, Vector3 v2, Vector3 v3, Vector2 p) const
{
	// Find the area of the the main triangle
	float mainTriArea = GetAreaOfTriangle(v1, v2, v3);

	// Convert from Vector 2 to Vector3 to be able to fit in GetAreaOfTriangle()
	Vector3 temp = Vector3(p.x, 0.0f, p.y);

	// Find alpha, beta and gamma
	float alpha = GetAreaOfTriangle(v1, v2, temp) / mainTriArea;
	float beta = GetAreaOfTriangle(v1, v3, temp) / mainTriArea;
	float gamma = 1.0f - alpha - beta;

	// Find the barycentre's y coordinate
	float barycentreY = (v1.y * alpha) + (v2.y * beta) + (v3.y * gamma);

	return barycentreY;
}

// Find the area of a triangle
float HeightMap::GetAreaOfTriangle(const Vector3 & p1, const Vector3 & p2, const Vector3 & p3) const
{
	// Calculate the sides of the triangle
	float a = sqrt(pow((p1.x - p2.x), 2) + pow((p1.z - p2.z), 2));
	float b = sqrt(pow((p2.x - p3.x), 2) + pow((p2.z - p3.z), 2));
	float c = sqrt(pow((p3.x - p1.x), 2) + pow((p3.z - p1.z), 2));
	// Then the semiperimeter of the triangle
	float s = (a + b + c) / 2;
	// And finally, use Heron's formula to calculate the area
	float area = sqrt(s * (s - a) * (s - b) * (s - c));

	return area;
}

float HeightMap::GetTerrainHeight(float particleWorldX, float particleWorldZ) const
{
	// The size of the heightmap divided by the size of the heightGrid array gives us the size of each grid square
	float gridSquareSize = (RAW_SIZE * HEIGHTMAP_X) / (float)GRID_SIZE;

	// Avoid possible division by 0 further on
	if (gridSquareSize < 1.0f)
	{
		return 0.0f;
	}

	// Find which grid square we're at
	int gridX = (int)floor(particleWorldX / gridSquareSize);
	int gridZ = (int)floor(particleWorldZ / gridSquareSize);

	// Check if the grid square position is actually valid
	if (gridX >= GRID_SIZE || gridZ >= GRID_SIZE || gridX < 0 || gridZ < 0)
	{
		return 0.0f;
	}

	// It's time to find where on the grid square we're at
	float coordX = (float)((int)particleWorldX % (int)gridSquareSize) / gridSquareSize;
	float coordZ = (float)((int)particleWorldZ % (int)gridSquareSize) / gridSquareSize;

	// Now that we have a grid square (and respectively 4 vertices - [0,0],[0,1],[1,0],[1,1]), it's time to find out which of the 2 triangles in-between we're at
	float h;

	// On the side of [0,0] -> [1,0] -> [0,1]
	if (coordX < (1 - coordZ))
	{
		// We now know the height of each of the vertices, as well as the position we're at in the triangle in-between. It's time to find the height of that exact position by using Barycentric interpolation.
		h = BarycentricInterpolation(Vector3(0.0f, heightGrid[gridX][gridZ], 0.0f),
			Vector3(1.0f, heightGrid[gridX + 1][gridZ], 0.0f),
			Vector3(0.0f, heightGrid[gridX][gridZ + 1], 1.0f),
			Vector2(coordX, coordZ));

	} // On the side of [0,1] -> [1,0] -> [1,1]
	else
	{
		h = BarycentricInterpolation(Vector3(1.0f, heightGrid[gridX + 1][gridZ], 0.0f),
			Vector3(1.0f, heightGrid[gridX + 1][gridZ + 1], 1.0f),
			Vector3(0.0f, heightGrid[gridX][gridZ + 1], 1.0f),
			Vector2(coordX, coordZ));
	}
	// Return the height of the terrain at the wanted position
	return h;
}
