// Code written for CSC8502 by Mark Kostadinov, Student Number 150368616

#include "stdafx.h"
#include "InterfaceManager.h"

InterfaceManager::InterfaceManager(Window &window, Renderer * renderer, SceneManager * sceneManager)
{
	SetWindow(&window);
	SetRenderer(renderer);
	SetSceneManager(sceneManager);
	SetInitialised(true);
}

InterfaceManager::~InterfaceManager()
{
}

void InterfaceManager::Update()
{
#ifdef DEBUG
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_C)) // Align the camera with the height level of the terrain
		GetRenderer()->GetCamera()->SetPosition(Vector3(GetRenderer()->GetCamera()->GetPosition().x, GetRenderer()->GetHeightMap()->GetTerrainHeight(GetRenderer()->GetCamera()->GetPosition().x, GetRenderer()->GetCamera()->GetPosition().z), GetRenderer()->GetCamera()->GetPosition().z));
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_V)) // Change the camera to point towards the origin
		GetRenderer()->GetCamera()->SetPosition(Vector3(-1000.0f, 1000.0f, -1000.0f));
#endif // DEBUG

	if (Window::GetKeyboard()->KeyHeld(KEYBOARD_8))
	{
		GetRenderer()->GetCamera()->SetPosition(GetRenderer()->GetMovingLight()->GetPosition());
	}

	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
	{
		// Go back to normal weather
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
		{
			GetRenderer()->ResetWeatherConditions();
		}
		// Let it snow
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
		{
			GetRenderer()->ResetWeatherConditions();
			GetRenderer()->SetRenderSnow(true);
		}
		// Let it rain
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
		{
			GetRenderer()->ResetWeatherConditions();
			GetRenderer()->SetRenderRain(true);
		}
	}
	else
		GetRenderer()->ResetWeatherConditions();
	// Toggle shadow debugging mode
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4))
		GetRenderer()->SetShadowDebuggingMode(!GetRenderer()->IsShadowDebuggingModeOn());
	// Toggle moving light mode
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5))
		GetRenderer()->SetMovingLightMode(!GetRenderer()->IsMovingLightModeOn());
	// Switch to the previous scene
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT))
		SwitchToPreviousScene();
	// Switch to the next scene
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT))
		SwitchToNextScene();
	// Toggle whether the program should cycle forward or backward through the scenes
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
		ToggleSceneSwitching();
	// Switch scenes automatically after a certain period of time has passed
	if (GetSceneManager()->IsSceneCyclingAllowed())
		SwitchScenesAutomatically();
}

void InterfaceManager::ToggleSceneSwitching()
{
	GetSceneManager()->SetSceneCyclingAllowed(!GetSceneManager()->IsSceneCyclingAllowed());
	GetSceneManager()->GetSceneTimer()->Reset();

#ifdef DEBUG
	if (GetSceneManager()->IsSceneCyclingAllowed())
		PrintToConsole("Cycling is currently allowed.", 1);
#endif // DEBUG
}

void InterfaceManager::SwitchScenesAutomatically()
{
#ifdef DEBUG
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Z))
		PrintToConsole("Scene timer: " + ToString(GetSceneManager()->GetSceneTimer()->GetMS()), 1);
#endif // DEBUG

	if (GetSceneManager()->GetSceneTimer()->GetMS() >= sceneCycleTimeMS)
		SwitchToNextScene();
}

void InterfaceManager::SwitchToNextScene()
{
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE && r->isFirstMiniScene || 
		GetSceneManager()->GetCurrentScene() == FIRST_SCENE && r->isSecondMiniScene)
		goto next;
	else
	{
		if (GetSceneManager()->GetCurrentScene() == FINAL_SCENE)
			GetSceneManager()->SetCurrentScene(FIRST_SCENE);
		else
			GetSceneManager()->SetCurrentScene(GetSceneManager()->GetCurrentScene() + 1);
	}
next:
	GetSceneManager()->GetSceneTimer()->Reset();
#ifdef DEBUG
	PrintToConsole("Current scene: " + to_string(GetSceneManager()->GetCurrentScene()), 1);
#endif // DEBUG
	r->UpdateCameraPosition();
}

void InterfaceManager::SwitchToPreviousScene()
{
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
		GetSceneManager()->SetCurrentScene(FINAL_SCENE);
	else
		GetSceneManager()->SetCurrentScene(GetSceneManager()->GetCurrentScene() - 1);
	GetSceneManager()->GetSceneTimer()->Reset();
#ifdef DEBUG
	PrintToConsole("Current scene: " + to_string(GetSceneManager()->GetCurrentScene()), 1);
#endif // DEBUG
	r->UpdateCameraPosition();
}
