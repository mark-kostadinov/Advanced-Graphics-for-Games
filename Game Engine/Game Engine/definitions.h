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
#include "../../FMOD/inc/fmod.hpp"
#include "../../FMOD/inc/fmod_errors.h"

// Debugging functions
#define DEBUG

// Window global variables
constexpr const char* programTitle = "CSC8502 Coursework";
constexpr int windowSizeX = 1200;
constexpr int windowSizeY = 800;
constexpr bool fullScreen = false;

// FMOD definitions
typedef FMOD::System System;
typedef FMOD::Channel Channel;
typedef FMOD::Sound Sound;

// Some GL_CLEAR colour definitions
#define GL_CLEAR_BLACK				0.0f, 0.0f, 0.0f, 1.0f
#define GL_CLEAR_GRAY				0.3f, 0.3f, 0.3f, 1.0f

// Time definitions
constexpr float sceneCycleTimeMS = 30000.0f; // i.e. 30 sec
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

// Light definitions
constexpr int movingLightsCount = 1;
constexpr float lightMovementDelta = 50.0f;

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