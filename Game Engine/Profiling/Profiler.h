// Code written for CSC8502 by Mark Kostadinov, Student Number 150368616
//
// This class handles the profiling and provides the user with some framerate statistics.

#pragma once

#include <math.h>
#include "../../nclgl/GameTimer.h"
#include "../../nclgl/OGLRenderer.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class Profiler
{
public:
	Profiler();
	~Profiler();

	void		Update();
	void		UpdateFrameRate(float elapsedTimeSec);
	void		ResetTimer();
	
	GameTimer*	GetPerformanceTimer() const			{ return perfTimer; }
	void		SetPerformanceTimer(GameTimer* g)	{ perfTimer = g; }
	float		GetElapsedTimeMilli() const			{ return elapsedTimeMilli; }
	void		SetElapsedTimeMilli(float f)		{ elapsedTimeMilli = f; }
	float		GetElapsedTimeSec() const			{ return elapsedTimeSec; }
	void		SetElapsedTimeSec(float f)			{ elapsedTimeSec = f; }
	int			GetFrameTime() const				{ return frameTime; }
	void		SetFrameTime(int i)					{ frameTime = i; }

	int			GetAverageFPS() const				{ return avgFPS; }
	void		SetAverageFPS(int i)				{ avgFPS = i; }
	int			GetFrames() const					{ return frames; }
	void		SetFrames(int i)					{ frames = i; }
	float		GetAccumTimeSec() const				{ return accumTimeSec; }
	void		SetAccumTimeSec(float f)			{ accumTimeSec = f; }

	int			GetUsageGPU() const					{ return gpuMemoryUsed; }
	void		SetUsageGPU(int i)					{ gpuMemoryUsed = i; }

	bool		HasInitialised() const { return init; }
	void		SetInitialised(bool b) { init = b; }

protected:
	// Performance timer
	GameTimer* perfTimer;
	float elapsedTimeMilli = 0.0f;
	float elapsedTimeSec = 0.0f;
	int frameTime = 0;

	// Average Framerate
	int avgFPS = 0;
	int frames = 0;
	float accumTimeSec = 0.0f;

	// CPU & GPU usage
	GLint gpuTotalMemoryKB;
	GLint gpuCurrentAvailableMemoryKB;
	int gpuMemoryUsed;
	int gpuCounter = 0;

	bool init;
};
