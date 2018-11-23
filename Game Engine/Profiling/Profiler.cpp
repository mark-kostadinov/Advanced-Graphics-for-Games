// Code written for CSC8502 by Mark Kostadinov, Student Number 150368616

#include "stdafx.h"
#include "Profiler.h"

Profiler::Profiler()
{
	perfTimer = new GameTimer();
	SetElapsedTimeMilli(0.0f);
	SetElapsedTimeSec(0.0f);
	SetFrameTime(0);

	SetAverageFPS(0);
	SetFrames(0);
	SetAccumTimeSec(0.0f);

	SetInitialised(true);
}

Profiler::~Profiler()
{
	delete perfTimer;
	perfTimer = NULL;
}

void Profiler::Update()
{
	// Stop the performance timer and store its value
	SetElapsedTimeMilli(GetPerformanceTimer()->GetMS());
	SetElapsedTimeSec(GetElapsedTimeMilli() / 1000);

	// Truncate the value for screen output
	SetFrameTime((int)trunc(GetElapsedTimeMilli()));

	// Calculate the average framerate
	UpdateFrameRate(GetElapsedTimeSec());

	// Capture the GPU memory usage once every 60 frames
	gpuCounter++;

	if (gpuCounter == 60)
	{
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &gpuTotalMemoryKB);
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &gpuCurrentAvailableMemoryKB);
		SetUsageGPU((gpuTotalMemoryKB - gpuCurrentAvailableMemoryKB) / 1000);
		
		gpuCounter = 0;
	}
}

void Profiler::UpdateFrameRate(float elapsedTimeSec)
{
	SetAccumTimeSec(GetAccumTimeSec() + elapsedTimeSec);

	if (GetAccumTimeSec() > 1.0f)
	{
		SetAverageFPS(GetFrames());

		SetFrames(0);
		SetAccumTimeSec(0.0f);
	}
	SetFrames(GetFrames() + 1);
}

void Profiler::ResetTimer()
{
	GetPerformanceTimer()->Reset();
}
