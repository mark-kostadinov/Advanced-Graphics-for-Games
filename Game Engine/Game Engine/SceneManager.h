#pragma once

#include "../../nclgl/GameTimer.h"
#include "definitions.h"

enum Labels
{
	FIRST_SCENE, SECOND_SCENE, FINAL_SCENE
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	GameTimer* GetSceneTimer() const { return sceneTimer; }

	int	GetCurrentScene() const { return currentScene; }
	void SetCurrentScene(int i) { currentScene = i; }
	bool HasInitialised() const { return init; }
	void SetInitialised(bool b) { init = b; }
	bool IsSceneCyclingAllowed() const { return allowSceneCycling; }
	void SetSceneCyclingAllowed(bool b) { allowSceneCycling = b; }

protected:
	GameTimer* sceneTimer;

	int currentScene;
	bool init;
	bool allowSceneCycling;
};
