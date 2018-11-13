// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616

#include "../../nclgl/Window.h"
#include "../Initialization/Initialiser.h"

#pragma comment(lib, "nclgl.lib")

int main()
{
	// Initialize the pseudo-random generator to a distinctive runtime value
	srand((unsigned int)time(0));

#ifdef DEBUG
	GLint extCount = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
	PrintToConsole("Number of extensions supported: " + to_string(extCount), 1);
#endif // DEBUG

	Window w(programTitle, windowSizeX, windowSizeY, fullScreen);
	w.LockMouseToWindow(true);
	w.ShowOSPointer(true);

	SceneManager sceneManager;
	Renderer renderer(w, &sceneManager);
	ResourceManager resourceManager(&renderer);
	resourceManager.LoadResources();
	InterfaceManager userInterface(w, &renderer, &sceneManager);
	Profiler profiler;
	Initialiser initialiser(w, &renderer, &resourceManager, &userInterface, &profiler, &sceneManager);

	// Check if all subsystems have been initialized and the all assets are loaded successfully
	if (!initialiser.AreSubSystemsInitialised() || !initialiser.HasInitialised() || !resourceManager.HasLoadedAssets())
	{
		PrintToConsole("Error. A subsystem has not been initialized or an asset has not been loaded properly.", 1);
		exit(1);
	}

	// Main loop
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		for (static bool resetOnce = false; resetOnce != true; resetOnce = true)
			userInterface.GetSceneManager()->GetSceneTimer()->Reset();

		float msec = w.GetTimer()->GetTimedMS();

		// Reset the performance timer every frame
		profiler.ResetTimer();

		// Update the different subsystems
		userInterface.Update();
		renderer.UpdateScene(msec);

		// Send the performance data to the renderer buffer
		renderer.textBuffer = to_string(profiler.GetFrameTime()) + ";" + to_string(profiler.GetAverageFPS()) + ";" + to_string(profiler.GetUsageGPU()) + ";";
		// And then render the scene
		renderer.RenderScene();

		profiler.Update();
	}
	return 0;
}
