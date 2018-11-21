// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616
//
// This class provides some useful definitions used throughout the program.

#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../nclgl/common.h"
#include "../../nclgl/Vector2.h"

// Debugging functions
//#define DEBUG

// Window global variables
constexpr const char* programTitle = "CSC8502 Coursework";
constexpr int windowSizeX = 1200;
constexpr int windowSizeY = 800;
constexpr bool fullScreen = false;

// Some GL_CLEAR colour definitions
#define GL_BLACK					0.0f, 0.0f, 0.0f, 1.0f
#define GL_GRAY						0.3f, 0.3f, 0.3f, 1.0f

// Time definitions
constexpr float sceneCycleTimeMS = 10000.0f; // i.e. 30 sec
constexpr float sceneTransitionTime = 5000.0f; // i.e. 5 sec
constexpr float sceneTransitionRate = 1.f / sceneTransitionTime;

// Particle emitter definitions
#define PARTICLE_EMITTER_POSITION	Vector3(0.0f, 7500.0f, 0.0f)
#define RAINDROP_COLOUR				Vector4(0.560784f, 0.560784f, 0.737255f, 0.9f) // Light Steel Blue
#define SNOWFLAKE_COLOUR			Vector4(1.0f, 1.0f, 1.0f, 1.0f)	// White
#define ORIGIN_CUBE_COLOUR			Vector4(1.0f, 0.0f, 0.0f, 1.0f) // Red

// Scene object definitions
constexpr int scenesCount	= 3;
#define ROCK_MTL_FILE_NAME			"Rock.mtl"
constexpr int numberOfTrees = 10;
constexpr int numberOfRocks = 5;

// Light & Camera definitions
constexpr float lightMovementDelta = 10.0f;
#define FIRST_SCENE_MAIN_LIGHT_POS			Vector3(-4000.0f, 10000.0f, -4000.0f)
#define SECOND_SCENE_MAIN_LIGHT_POS			Vector3(-30000.0f, 2000.0f, -10000.0f)
#define FIRST_MINI_SCENE_MAIN_LIGHT_POS		Vector3(0.0f, 30000.0f, 0.0f) /// TODO
#define SECOND_MINI_SCENE_MAIN_LIGHT_POS	Vector3(10000.0f, 10000.0f, 0.0f) /// TODO
#define FIRST_SCENE_MOVING_LIGHT_Y			25000.0f
#define SECOND_SCENE_MOVING_LIGHT_Y			3000.0f
#define FIRST_SCENE_CAM_POS					Vector3(5000.0f, 7000.0f, 22000.0f)
#define SECOND_SCENE_CAM_POS				Vector3(7000.0f, 2500.0f, -12000.0f)
#define SUN_STARTING_POS					Vector3(0.0f, 1500.0f, 0.0f)

// Printing out function
template <typename T>
inline void PrintToConsole(T element, int numNewLines = NULL)
{
	std::string newLines;
	for (int i = 0; i < numNewLines; i++)
		newLines += "\n";
	std::cout << element << newLines;
}

// Thing to string function
template <typename T>
inline std::string ToString(const T& value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}

template <typename T>
inline void ClearSTLVector(std::vector<T> vector)
{
	vector.clear();
	std::vector<T> temp;
	vector.swap(temp);
}

static Vector2 GenerateRandomPosition(int maxX, int maxZ)
{
	int x = rand() % maxX;
	int z = rand() % maxZ;
	Vector2 result = Vector2((float)x, (float)z);

	return result;
}

#define SHADOWSIZE 4096