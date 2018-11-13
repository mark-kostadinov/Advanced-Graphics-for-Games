// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616

#include "stdafx.h"
#include "Initialiser.h"

Initialiser::Initialiser(Window & w, Renderer* renderer, ResourceManager* resourceManager, InterfaceManager* interfaceManager, Profiler* profiler, SceneManager * sceneManager)
{
	SetWindow(&w);
	SetRenderer(renderer);
	SetResourceManager(resourceManager);
	SetInterfaceManager(interfaceManager);
	SetProfiler(profiler);
	SetSceneManager(sceneManager);

	SetInitialised(true);
}

Initialiser::~Initialiser()
{
}

bool Initialiser::AreSubSystemsInitialised()
{
	if (!GetWindow()->HasInitialised() || !GetSceneManager()->HasInitialised() || !GetRenderer()->HasInitialised() ||
		!GetResourceManager()->HasInitialised() || !GetInterfaceManager()->HasInitialised() || !GetProfiler()->HasInitialised())
	{
		return false;
	}
	return true;
}
