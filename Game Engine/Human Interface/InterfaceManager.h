// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616
//
// This class handles human interface detection and response.

#pragma once

#include "../Graphics/Renderer.h"

class InterfaceManager
{
public:
	InterfaceManager(Window &window, Renderer * renderer, SceneManager* sceneManager);
	~InterfaceManager();

	void			Update();

	bool			HasInitialised() const		{ return init; }
	void			SetInitialised(bool b)		{ init = b; }

	Window*			GetWindow() const			{ return w; }
	void			SetWindow(Window* w)		{ this->w = w; }
	Renderer*		GetRenderer() const			{ return r; }
	void			SetRenderer(Renderer* r)	{ this->r = r; }
	SceneManager*	GetSceneManager() const						{ return sceneManager; }
	void			SetSceneManager(SceneManager* sceneManager) { this->sceneManager = sceneManager; }

protected:
	void ToggleSceneSwitching();
	void SwitchScenesAutomatically();
	void SwitchToNextScene();
	void SwitchToPreviousScene();

	Window * w;
	Renderer * r;
	SceneManager * sceneManager;

	bool init;
};
