#include "SceneManager.h"

SceneManager::SceneManager()
{
	SetCurrentScene(FIRST_SCENE);
#ifndef DEBUG
	SetSceneCyclingAllowed(true);
#endif // DEBUG
#ifdef DEBUG
	SetSceneCyclingAllowed(false);
#endif // DEBUG
	sceneTimer = new GameTimer();
	SetInitialised(true);
}

SceneManager::~SceneManager()
{
	delete sceneTimer;
	sceneTimer = NULL;
}
