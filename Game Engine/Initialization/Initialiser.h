// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616
//
// This class handles the successful initialization of the program.

#pragma once

#include "../Resource Management/ResourceManager.h"
#include "../Profiling/Profiler.h"
#include "../Human Interface/InterfaceManager.h"

class Initialiser
{
public:
	Initialiser(Window & w, Renderer* renderer, ResourceManager* resourceManager, InterfaceManager* interfaceManager, Profiler* profiler, SceneManager * sceneManager);
	~Initialiser();

	bool				AreSubSystemsInitialised();

	Window*				GetWindow() const { return w; }
	void				SetWindow(Window* w) { this->w = w; }
	Renderer*			GetRenderer() const { return r; }
	void				SetRenderer(Renderer* r) { this->r = r; }
	ResourceManager*	GetResourceManager() const { return rm; }
	void				SetResourceManager(ResourceManager* rm) { this->rm = rm; }
	InterfaceManager*	GetInterfaceManager() const { return im; }
	void				SetInterfaceManager(InterfaceManager* im) { this->im = im; }
	Profiler*			GetProfiler() const { return profiler; }
	void				SetProfiler(Profiler* profiler) { this->profiler = profiler; }
	SceneManager*		GetSceneManager() const { return sceneManager; }
	void				SetSceneManager(SceneManager* sceneManager) { this->sceneManager = sceneManager; }

	bool				HasInitialised() const { return init; }
	void				SetInitialised(bool b) { init = b; }

protected:
	Window* w;
	Renderer* r;
	ResourceManager* rm;
	InterfaceManager* im;
	Profiler* profiler;
	SceneManager* sceneManager;

	bool init;
};
