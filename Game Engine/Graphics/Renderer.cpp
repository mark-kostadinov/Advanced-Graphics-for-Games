#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(Window &parent, SceneManager * sceneManager) : OGLRenderer(parent)
{
	SetWindow(&parent);
	SetSceneManager(sceneManager);

	// Set the projection
	projMatrix = Matrix4::Perspective(nearPlane, farPlane, (float)width / (float)height, fov);

	// Initialize the camera
	camera = new Camera(0.0f, 0.0f, FIRST_SCENE_CAM_POS);

	// Initialize the main light
	SetMainLight(new Light(FIRST_SCENE_MAIN_LIGHT_POS, Vector4(1, 1, 1, 1), RAW_HEIGHT * HEIGHTMAP_X * 6.0f));

	// Generate a moving light for the rain scene
	Vector2 randomPosition = GenerateRandomPosition((int)(RAW_HEIGHT * HEIGHTMAP_X), (int)(RAW_WIDTH * HEIGHTMAP_Z));
	SetMovingLight(new Light(Vector3(randomPosition.x, FIRST_SCENE_MOVING_LIGHT_Y, randomPosition.y), Vector4(1, 1, 1, 1), RAW_HEIGHT * HEIGHTMAP_X * 5.0f));

	// Create the Frame Buffer Object
	GenerateFBOs();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void)
{
	DeleteObjects();
	DeleteShaders();
	DeleteTextures();
}

void Renderer::UpdateScene(float msec)
{
	this->msec = msec;

	camera->UpdateCamera();
	viewMatrix = camera->BuildViewMatrix();
#ifdef CINEMATIC_CAM_MOVEMENT
	viewMatrix = Matrix4::BuildViewMatrix(camera->GetPosition(), camera->GetDirection());
#endif // CINEMATIC_CAM_MOVEMENT
	frameFrustum.FromMatrix(projMatrix*viewMatrix);

	SwitchScenesUpdating();
}

void Renderer::RenderScene()
{
	glClearColor(GL_BLACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// If it is time to change the scene, apply some post-processing effects on it
	if (GetSceneManager()->IsSceneCyclingAllowed() && 
		GetSceneManager()->GetSceneTimer()->GetMS() >= sceneCycleTimeMS - sceneTransitionTime)
	{
		RenderSceneFBO();

		// Have to split it up due to the use of another FBO in some methods
		if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
			RenderWeather();

		DrawPostProcess();
		ApplyPostProcessEffects();
	}
	else
	{
		DrawShadows();
		DrawScene();
		ResetPPFX();
	}
	RenderText(textBuffer);

	SwapBuffers();
	ClearNodeLists();
	glUseProgram(0);
}

void Renderer::DrawScene()
{
	DrawSkybox();
	SwitchScenesRendering();
	SortNodeLists();
	DrawNodes();
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(GL_FRONT); // Solves shadow acne
	castShadows = true;

	Light currentLight;
	if (IsMovingLightModeOn())
		currentLight = *movingLight;
	else
		currentLight = *mainLight;

	Matrix4 tempViewMatrix = viewMatrix;
	Matrix4 tempProjMatrix = projMatrix;
	projMatrix = Matrix4::Perspective(1.0f, 30000.0f, 1, 90.0f);

	if (IsMovingLightModeOn())
		viewMatrix = Matrix4::BuildViewMatrix(currentLight.GetPosition(), Vector3(0.0f, 0.0f, 0.0f));
	else
		viewMatrix = Matrix4::BuildViewMatrix(currentLight.GetPosition(), Vector3(RAW_HEIGHT * HEIGHTMAP_X, 0.0f, RAW_WIDTH * HEIGHTMAP_Z));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);
	fboInUse = true;
	DrawScene();
	fboInUse = false;
	ClearNodeLists();

	projMatrix = tempProjMatrix;
	viewMatrix = tempViewMatrix;

	castShadows = false;
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glCullFace(GL_BACK);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ApplyPostProcessEffects()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(defaultShader);
	SwitchToOrtho();
	UpdateShaderMatrices();
	versatileMesh->SetTexture(bufferColourTexture[0]);
	versatileMesh->Draw();
	ResetMVP();
	glUseProgram(0);
}

void Renderer::ResetPPFX()
{
	blurFactor = 1.0f;
	alphaFade = 1.0f;
}

void Renderer::DrawOmniDirShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, omniShadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(GL_FRONT); // Solves shadow acne
	castOmniShadows = true;

	Light currentLight;
	if (IsMovingLightModeOn())
		currentLight = *movingLight;
	else
		currentLight = *mainLight;

	Matrix4 tempProjMatrix = projMatrix;
	Matrix4 tempViewMatrix = viewMatrix;
	projMatrix = Matrix4::Perspective(1.0f, 30000.0f, 1, 90.0f);

	Vector3 lightSourcePosition = currentLight.GetPosition();
	Vector3 cameraDirections[6] = { Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(0, 1, 0),
									Vector3(0, -1, 0), Vector3(0, 0, 1), Vector3(0, 0, -1) };
	Vector3 upDirections[6] = { Vector3(0, -1, 0),Vector3(0, -1, 0), Vector3(0, 0, 1),
								Vector3(0, 0, -1), Vector3(0, -1, 0), Vector3(0, -1, 0) };

	for (int i = 0; i < 6; i++)
	{
		// Generate the 6 viewProjection matrices from the camera's position
		viewMatrix = Matrix4::BuildViewMatrix(lightSourcePosition, lightSourcePosition + cameraDirections[i], upDirections[i]);
		lightMatrices[i] = (projMatrix * viewMatrix);
	}
	fboInUse = true;
	DrawScene();
	fboInUse = false;
	ClearNodeLists();

	projMatrix = tempProjMatrix;
	viewMatrix = tempViewMatrix;

	castOmniShadows = false;
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glCullFace(GL_BACK);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadows()
{
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE || (GetSceneManager()->GetCurrentScene() == SECOND_SCENE && !movingLightMode))
		DrawShadowScene();
	else if (GetSceneManager()->GetCurrentScene() == SECOND_SCENE && movingLightMode)
		DrawOmniDirShadowScene();
}

void Renderer::RenderSceneFBO()
{
	DrawShadows();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fboInUse = true;
	DrawScene();
	fboInUse = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenerateFBOs()
{
	GenerateDepthTextureFBO(snowBufferDepthTexture);
	GenerateDepthTextureFBO(rainBufferDepthTexture);
	GenerateDepthTextureFBO(postProcessingDepthTexture);
	GenerateDepthTextureFBO(bufferDepthTexture);

	// Generate a Shadow map texture
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate an Omni-Directional Shadow cubeMap texture
	glGenTextures(1, &omniShadowCubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, omniShadowCubeMapTexture);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
			SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GenerateColourTextureFBO(snowBufferColourTexture);
	GenerateColourTextureFBO(rainBufferColourTexture);
	GenerateColourTextureFBO(postProcessingColourTexture);
	for (int i = 0; i < 2; ++i)
		GenerateColourTextureFBO(bufferColourTexture[i]);

	glGenFramebuffers(1, &snowCollisionFBO);
	glGenFramebuffers(1, &rainCollisionFBO);
	glGenFramebuffers(1, &postProcessingFBO);
	glGenFramebuffers(1, &bufferFBO);

	// Shadows
	glGenFramebuffers(1, &omniShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, omniShadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, omniShadowCubeMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	AttachDepthTextureFBO(shadowFBO, shadowTexture);

	AttachTexturesFBO(snowCollisionFBO, snowBufferColourTexture, snowBufferDepthTexture);
	AttachTexturesFBO(rainCollisionFBO, rainBufferColourTexture, rainBufferDepthTexture);
	AttachTexturesFBO(postProcessingFBO, postProcessingColourTexture, postProcessingDepthTexture);
	AttachTexturesFBO(bufferFBO, bufferColourTexture[0], bufferDepthTexture);
}

void Renderer::GenerateDepthTextureFBO(GLuint &texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void Renderer::GenerateColourTextureFBO(GLuint &texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void Renderer::AttachTexturesFBO(GLuint & fbo, GLuint & colourTexture, GLuint & depthTexture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(GL_BLACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !depthTexture || !colourTexture)
	{
		PrintToConsole("Error: Framebuffer is not complete!", 1);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::AttachDepthTextureFBO(GLuint & fbo, GLuint & depthTexture)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawParticleCollisionFBO()
{
	if (snowEmitter != NULL)
		glBindFramebuffer(GL_FRAMEBUFFER, snowCollisionFBO);
	if (rainEmitter != NULL)
		glBindFramebuffer(GL_FRAMEBUFFER, rainCollisionFBO);
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	if (snowEmitter != NULL)
		SetCurrentShader(snowCollisionShader);
	if (rainEmitter != NULL)
		SetCurrentShader(rainCollisionShader);
	modelMatrix.ToIdentity();
	SwitchToOrtho();

	if (snowEmitter != NULL)
	{
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particlePosX"), snowEmitter->GetFallenSnowflakePosX());
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particlePosZ"), snowEmitter->GetFallenSnowflakePosZ());
	}
	if (rainEmitter != NULL)
	{
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particlePosX"), rainEmitter->GetFallenRaindropPosX());
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particlePosZ"), rainEmitter->GetFallenRaindropPosZ());
	}
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "terrainSize"), RAW_SIZE);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "terrainGridSize"), HEIGHTMAP_X);
	UpdateShaderMatrices();

	weatherEffectsMesh->Draw();

	ResetMVP();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawPostProcess()
{
	// Fade out the scene by progressively lowering its alpha value
	alphaFade += - sceneTransitionRate * msec;
	if (alphaFade < 0.0f)
		alphaFade = 0.0f;

	blurFactor += 0.02f;

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTexture[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	SetCurrentShader(postProcessingShader);
	SwitchToOrtho();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isFadingOut"), 0);
	glUniform1f(glGetUniformLocation(GetCurrentShader()->GetProgram(), "alphaFade"), alphaFade);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < (int)blurFactor; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTexture[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);
	
		versatileMesh->SetTexture(bufferColourTexture[0]);
		versatileMesh->Draw();
		// Now to swap the colour buffers and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTexture[0], 0);
		
		versatileMesh->SetTexture(bufferColourTexture[1]);
		versatileMesh->Draw();
	}
	// Turn on fading out
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isFadingOut"), 1);
	versatileMesh->Draw();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderWeather()
{
	// Only if we render one of the weather conditions on screen that we want the ground to be covered with it
	if (renderSnow && snowEmitter != NULL)
		DrawParticleCollisionFBO();
	if (renderRain && rainEmitter != NULL)
		DrawParticleCollisionFBO();
}

void Renderer::UpdateWeather()
{
	// Update each weather condition
	UpdateSnow();
	UpdateRain();

	// Then prepare for snowing or raining by updating the emitters accordingly
	if (snowEmitter != NULL)
		snowEmitter->Update(msec, heightMap);
	if (rainEmitter != NULL)
		rainEmitter->Update(msec, heightMap);
}

void Renderer::ResetWeatherConditions()
{
	if (renderSnow || renderRain)
		ClearAccumulation();
	renderSnow = false;
	renderRain = false;
}

void Renderer::UpdateSnow()
{
	// If snow is toggled OFF and there's still an active snow emitter
	if (!renderSnow && GetSnowEmitter() != NULL)
	{
		// Remove the snow emitter node from the rendering pipeline
		for (auto it = rootNodes[FIRST_SCENE]->GetChildIteratorStart(); it != rootNodes[FIRST_SCENE]->GetChildIteratorEnd(); ++it)
		{
			if ((*it) == snowEmitterNode)
			{
				rootNodes[FIRST_SCENE]->RemoveChild(snowEmitterNode);
				break;
			}
		}
		// Then delete the snow emitter
		delete snowEmitter;
		snowEmitter = NULL;
		snowOnce = true;
	}
	// If snow is toggled ON and there's still an active snow emitter
	if (renderSnow && GetSnowEmitter() != NULL)
	{
		snowEmitterNode->SetMesh(GetSnowEmitter());
		// Add the snow emitter back to the rendering pipeline
		if (snowOnce)
			rootNodes[FIRST_SCENE]->AddChild(snowEmitterNode);
		snowOnce = false;
	}
	// If snow is toggled ON and there's no active snow emitter
	if (renderSnow && GetSnowEmitter() == NULL)
	{
		// Create one
		SetSnowEmitter(new SnowEmitter());
		GetSnowEmitter()->SetEmitterPosition(PARTICLE_EMITTER_POSITION);
		snowEmitterNode->SetMesh(GetSnowEmitter());
		// And add it back to the rendering pipeline
		if (snowOnce)
			rootNodes[FIRST_SCENE]->AddChild(snowEmitterNode);
		snowOnce = false;
	}
}

void Renderer::UpdateRain()
{
	// If rain is toggled OFF and there's still an active rain emitter
	if (!renderRain && GetRainEmitter() != NULL)
	{
		// Remove the rain emitter node from the rendering pipeline
		for (auto it = rootNodes[FIRST_SCENE]->GetChildIteratorStart(); it != rootNodes[FIRST_SCENE]->GetChildIteratorEnd(); ++it)
		{
			if ((*it) == rainEmitterNode)
			{
				rootNodes[FIRST_SCENE]->RemoveChild(rainEmitterNode);
				break;
			}
		}
		// Then delete the rain emitter
		delete rainEmitter;
		rainEmitter = NULL;
		rainOnce = true;
	}
	// If rain is toggled ON and there's still an active rain emitter
	if (renderRain && GetRainEmitter() != NULL)
	{
		rainEmitterNode->SetMesh(GetRainEmitter());
		// Add the rain emitter back to the rendering pipeline
		if (rainOnce)
			rootNodes[FIRST_SCENE]->AddChild(rainEmitterNode);
		rainOnce = false;
	}
	// If rain is toggled ON and there's no active rain emitter
	if (renderRain && GetRainEmitter() == NULL)
	{
		// Create one
		SetRainEmitter(new RainEmitter());
		GetRainEmitter()->SetEmitterPosition(PARTICLE_EMITTER_POSITION);
		rainEmitterNode->SetMesh(GetRainEmitter());
		// And add it back to the rendering pipeline
		if (rainOnce)
			rootNodes[FIRST_SCENE]->AddChild(rainEmitterNode);
		rainOnce = false;
	}
}

void Renderer::ClearAccumulation()
{
	if (renderSnow)
		glBindFramebuffer(GL_FRAMEBUFFER, snowCollisionFBO);
	if (renderRain)
		glBindFramebuffer(GL_FRAMEBUFFER, rainCollisionFBO);
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);

	SetCurrentShader(defaultShader);
	modelMatrix.ToIdentity();
	SwitchToOrtho();

	UpdateShaderMatrices();

	weatherEffectsMesh->Draw();

	ResetMVP();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::ResetMVP()
{
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Perspective(nearPlane, farPlane, (float)width / (float)height, fov);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::SwitchToOrtho()
{
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
}

void Renderer::DeleteObjects()
{
	// FIRST_SCENE
	delete camera;
	camera = NULL;
	delete heightMap;
	heightMap = NULL;
	delete snowEmitter;
	snowEmitter = NULL;
	delete rainEmitter;
	rainEmitter = NULL;

	delete versatileMesh;
	versatileMesh = NULL;
	delete weatherEffectsMesh;
	weatherEffectsMesh = NULL;
	delete originCubeMesh;
	originCubeMesh = NULL;
	delete skyMesh;
	skyMesh = NULL;
	delete treeMesh;
	treeMesh = NULL;
	delete rockMesh;
	rockMesh = NULL;
	// SECOND_SCENE
	delete earthMesh;
	earthMesh = NULL;
	delete reflectiveSphereMesh;
	reflectiveSphereMesh = NULL;
	delete sunMesh;
	sunMesh = NULL;
	delete moonMesh;
	moonMesh = NULL;

	for (int i = 0; i < scenesCount; i++)
	{
		delete rootNodes[i];
		rootNodes[i] = NULL;
	}

	if (!treeNodes.empty())
	{
		for (auto it = treeNodes.begin(); it != treeNodes.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		ClearSTLVector(treeNodes);
	}
	if (!rockNodes.empty())
	{
		for (auto it = rockNodes.begin(); it != rockNodes.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		ClearSTLVector(rockNodes);
	}

	delete mainLight;
	mainLight = NULL;
	delete movingLight;
	movingLight = NULL;

	delete dataFont;
	dataFont = NULL;
}

void Renderer::DeleteShaders()
{
	// FIRST_SCENE
	delete defaultShader;
	defaultShader = NULL;
	delete sceneNodeShader;
	sceneNodeShader = NULL;
	delete sceneObjectShader;
	sceneObjectShader = NULL;
	delete skyboxShader;
	skyboxShader = NULL;
	delete heightMapShader;
	heightMapShader = NULL;
	delete snowParticleShader;
	snowParticleShader = NULL;
	delete rainParticleShader;
	rainParticleShader = NULL;
	delete snowCollisionShader;
	snowCollisionShader = NULL;
	delete rainCollisionShader;
	rainCollisionShader = NULL;
	delete postProcessingShader;
	postProcessingShader = NULL;
	delete fontShader;
	fontShader = NULL;
	// SECOND_SCENE
	delete shadowShader;
	shadowShader = NULL;
	delete omniShadowShader;
	omniShadowShader = NULL;
	delete sceneObjectOmniShader;
	sceneObjectOmniShader = NULL;
	delete reflectionShader;
	reflectionShader = NULL;

	currentShader = 0;
}

void Renderer::DeleteTextures()
{
	// FIRST_SCENE
	glDeleteTextures(1, &cubeMapSunnyTexture);
	glDeleteTextures(1, &cubeMapCloudyTexture);
	glDeleteTextures(1, &heightMapTexture);
	glDeleteTextures(1, &heightMapBumpTexture);
	glDeleteTextures(1, &snowBufferColourTexture);
	glDeleteTextures(1, &snowBufferDepthTexture);
	glDeleteTextures(1, &rainBufferColourTexture);
	glDeleteTextures(1, &rainBufferDepthTexture);
	glDeleteTextures(1, &postProcessingColourTexture);
	glDeleteTextures(1, &postProcessingDepthTexture);
	glDeleteTextures(2, bufferColourTexture);
	glDeleteTextures(1, &bufferDepthTexture);
	glDeleteFramebuffers(1, &snowCollisionFBO);
	glDeleteFramebuffers(1, &rainCollisionFBO);
	glDeleteFramebuffers(1, &postProcessingFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	// SECOND_SCENE
	glDeleteTextures(1, &shadowTexture);
	glDeleteTextures(1, &cubeMapSpaceTexture);
	glDeleteTextures(1, &earthTexture);
	glDeleteTextures(1, &omniShadowCubeMapTexture);
	glDeleteTextures(1, &sunTexture);
	glDeleteTextures(1, &moonTexture);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &omniShadowFBO);
}

void Renderer::UpdateLights()
{
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
	{
		mainLight->SetPosition(FIRST_SCENE_MAIN_LIGHT_POS);
		mainLight->SetColour(Vector4(1, 1, 1, 1));
		mainLight->SetRadius(RAW_HEIGHT * HEIGHTMAP_X * 6.0f);

		movingLight->SetPosition(Vector3(movingLight->GetPosition().x, FIRST_SCENE_MOVING_LIGHT_Y, movingLight->GetPosition().z));
		movingLight->SetRadius(RAW_HEIGHT * HEIGHTMAP_X * 5.0f);
	}
	else if (GetSceneManager()->GetCurrentScene() == SECOND_SCENE)
	{
		mainLight->SetPosition(SECOND_SCENE_MAIN_LIGHT_POS);
		mainLight->SetColour(Vector4(0.95f, 1.0f, 0.9f, 1.0f));
		mainLight->SetRadius(RAW_HEIGHT * HEIGHTMAP_X * 3.0f);

		movingLight->SetPosition(Vector3(movingLight->GetPosition().x, SECOND_SCENE_MOVING_LIGHT_Y, movingLight->GetPosition().z));		
		movingLight->SetRadius(30000.0f);
	}
}

void Renderer::UpdateMovingLight(Light * l, float delta, bool orbit)
{
	if (orbit)
	{
		float radius = 15000.0f;
		float x = sin(planetRotationAngle * 0.05f) * (radius - 1000.0f);
		float z = cos(planetRotationAngle * 0.05f) * (radius - 1000.0f);
		float sunX = sin(planetRotationAngle * 0.05f) * radius;
		float sunZ = cos(planetRotationAngle * 0.05f) * radius;

		l->SetPosition(Vector3(x, l->GetPosition().y, z));
		GetSunNode()->SetTransform(Matrix4::Translation(Vector3(sunX, GetSunNode()->GetTransform().GetPositionVector().y, sunZ)));
	}
	else
	{
		l->SetMovementCounter(l->GetMovementCounter() + delta);
		l->SetPosition(Vector3(l->GetMovementCounter(), l->GetPosition().y, l->GetMovementCounter()));

		if (l->GetMovementCounter() > RAW_HEIGHT * HEIGHTMAP_X)
		{
			l->SetMovementCounter(0.0f);
			l->SetPosition(Vector3(l->GetMovementCounter(), l->GetPosition().y, l->GetMovementCounter()));
		}
	}
}

void Renderer::UpdatePlanets()
{
	GetEarthNode()->SetTransform(Matrix4::Translation(GetEarthNode()->GetTransform().GetPositionVector()) * Matrix4::Rotation(planetRotationAngle, Vector3(0.0f, 1.0f, 0.0f)));
	GetMoonNode()->SetTransform(Matrix4::Translation(GetMoonNode()->GetTransform().GetPositionVector()) * Matrix4::Rotation(planetRotationAngle, Vector3(1.0f, 1.0f, 1.0f)));

	float radius = 10000.0f;
	float x = cos(planetRotationAngle * 0.05f) * radius;
	float z = sin(planetRotationAngle * 0.05f) * radius;

	GetReflectiveSphereNode()->SetTransform(Matrix4::Translation(Vector3(x, GetReflectiveSphereNode()->GetTransform().GetPositionVector().y, z)));

	planetRotationAngle += 0.1f;
	if (planetRotationAngle > 360.0f)
		planetRotationAngle = 0.1f;
}

void Renderer::UpdateCameraPosition()
{
	// A really hacky way of doing this but it saves a lot of code duplication since 
	// I'm just showing different parts/effects of the same scene
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE && isFirstMiniScene)
	{
		ResetWeatherConditions();
		renderSnow = true;
		isFirstMiniScene = false;
		isSecondMiniScene = true;
		camera->SetPosition(FIRST_MINI_SCENE_CAM_POS);
		camera->SetPitch(-20.0f);
		camera->SetYaw(275.0f);
	}
	else if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE && isSecondMiniScene)
	{
		ResetWeatherConditions();
		renderRain = true;
		isSecondMiniScene = false;
		camera->SetPosition(SECOND_MINI_SCENE_CAM_POS);
		camera->SetPitch(-10.0f);
		camera->SetYaw(-185.0f);
	}
	else if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE && !isFirstMiniScene) 
	{
		isFirstMiniScene = true;
		camera->SetPosition(FIRST_SCENE_CAM_POS);
		camera->SetPitch(0.0f);
		camera->SetYaw(0.0f);
	}
	else if (GetSceneManager()->GetCurrentScene() == SECOND_SCENE)
	{
		camera->SetPosition(SECOND_SCENE_CAM_POS);
		camera->SetPitch(-10.0f);
		camera->SetYaw(-250.0f);
	}
}

void Renderer::RenderText(const string & text)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	SetCurrentShader(GetFontShader());

	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
	UpdateShaderMatrices();

	// Split the text into tokens
	vector<string> v;
	SplitText(v, text);
	Vector3 screenTextVector = Vector3(0, 4.0f, 0);
	Vector3 screenTextPaddingVector = Vector3(0, 20.0f, 0);

	// Render functions to encapsulate the font rendering
	for (int i = 0; i < (int)v.size(); i++)
	{
		DrawText("Frame time: " + v.at(i) + "ms", screenTextVector + screenTextPaddingVector * (float)i, 16.0f);
		i++;
		DrawText("Frame rate: " + v.at(i) + "FPS", screenTextVector + screenTextPaddingVector * (float)i, 16.0f);
		i++;
		DrawText("GPU memory used: " + v.at(i) + "MB", screenTextVector + screenTextPaddingVector * (float)i, 16.0f);
	}
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
	{
		DrawText("Press [1] to reset the weather", screenTextVector + screenTextPaddingVector * (float)v.size(), 16.0f);
		DrawText("Press [2] to toggle snow", screenTextVector + screenTextPaddingVector * (float)((int)v.size() + 1), 16.0f);
		DrawText("Press [3] to toggle rain", screenTextVector + screenTextPaddingVector * (float)((int)v.size() + 2), 16.0f);
		DrawText("Press [4] to toggle shadow debug mode", screenTextVector + screenTextPaddingVector * (float)((int)v.size() + 3), 16.0f);
		DrawText("Press [5] to toggle moving light mode", screenTextVector + screenTextPaddingVector * (float)((int)v.size() + 4), 16.0f);
	}
	if (GetSceneManager()->GetCurrentScene() == SECOND_SCENE || GetSceneManager()->GetCurrentScene() == FINAL_SCENE)
	{
		DrawText("Press [4] to toggle shadow debug mode", screenTextVector + screenTextPaddingVector * (float)v.size(), 16.0f);
		DrawText("Press [5] to toggle moving light mode", screenTextVector + screenTextPaddingVector * (float)((int)v.size() + 1), 16.0f);
	}

	ResetMVP();
	glUseProgram(0);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawText(const string &text, const Vector3 &position, const float size, const bool perspective)
{
	TextMesh* textMesh = new TextMesh(text, *dataFont);

	if (perspective)
	{
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 20000.0f, (float)width / (float)height, 45.0f);
	}
	else
	{
		// In ortho mode, we subtract the y from the height, so that a height of 0
		// is at the top left of the screen, which is more intuitive
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	UpdateShaderMatrices();

	textMesh->Draw();

	delete textMesh;
}

void Renderer::SplitText(vector<string>& v, const string & text)
{
	string delimiter = ";";
	string token;

	int start = 0;
	int end = text.find(delimiter);
	while (end != string::npos)
	{
		token = text.substr(start, end - start);
		v.push_back(token);

		start = end + delimiter.length();
		end = text.find(delimiter, start);
	}
}

void Renderer::DrawNode(SceneNode*n)
{
	if (n->GetMesh())
	{
		// Check if the node is supposed to be rendered
		if (n->GetMesh() == snowEmitter && !renderSnow)
			return;
		if (n->GetMesh() == rainEmitter && !renderRain)
			return;

		if (castShadows)
		{
			SetCurrentShader(shadowShader);
			// Swap between the two shaders
			GetEarthNode()->SetShader(GetSceneObjectShader());
			GetSunNode()->SetShader(GetSceneObjectShader());
			GetMoonNode()->SetShader(GetSceneObjectShader());
		}
		else if (castOmniShadows)
		{
			SetCurrentShader(omniShadowShader);
			// Swap between the two shaders
			GetEarthNode()->SetShader(GetSceneObjectOmniShader());
			GetSunNode()->SetShader(GetSceneObjectOmniShader());
			GetMoonNode()->SetShader(GetSceneObjectOmniShader());
		}
		else
			SetCurrentShader(n->GetShader());

		SetLighting(n);
		SetHeightMapBufferTexture(n);
		SetUniforms(n);

		n->Draw();
		glUseProgram(0);
	}
}

void Renderer::SetUniforms(SceneNode* n)
{
	if (n->GetMesh() == GetSnowEmitter())
		SetShaderParticleSize(GetSnowEmitter()->GetParticleSize());
	if (n->GetMesh() == GetRainEmitter())
		SetShaderParticleSize(GetRainEmitter()->GetParticleSize());

	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "bumpTex"), 1);
	if (n->GetMesh()->GetBumpMap())
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "hasBumpTexture"), 1);
	else
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "hasBumpTexture"), 0);
	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
	glUniform3fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
	if (renderSnow)
	{
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "renderSnow"), true);
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "snowBufferColourTexture"), 2);
	}
	if (renderRain)
	{
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "renderRain"), true);
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "rainBufferColourTexture"), 3);
	}
	glUniform1f(glGetUniformLocation(GetCurrentShader()->GetProgram(), "terrainSize"), RAW_SIZE);
	glUniform1f(glGetUniformLocation(GetCurrentShader()->GetProgram(), "terrainGridSize"), HEIGHTMAP_X);

	// Reflections
	if (n->GetMesh() == reflectiveSphereMesh)
	{
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "cubeTex"), 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyMesh->GetTexture());
	}
	// Shadows
	if (n->GetMesh() == heightMap || n->GetMesh() == treeMesh || n->GetMesh() == rockMesh || n->GetMesh() == earthMesh ||
		n->GetMesh() == reflectiveSphereMesh || n->GetMesh() == sunMesh || n->GetMesh() == moonMesh)
	{
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "shadowTex"), 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadowTexture);

		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "omniShadowMap"), 6);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, omniShadowCubeMapTexture);
	}

	modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
	UpdateShaderMatrices();

	// Shadows
	if (n->GetMesh() == heightMap || n->GetMesh() == treeMesh || n->GetMesh() == rockMesh || n->GetMesh() == earthMesh ||
		n->GetMesh() == reflectiveSphereMesh || n->GetMesh() == sunMesh || n->GetMesh() == moonMesh)
	{
		Matrix4 tempMatrix = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "shadowMatrix"), 1, false, tempMatrix.values);

		if (castOmniShadows)
		{
			for (int i = 0; i < 6; i++)
			{
				string smi = "lightMatrices[" + to_string(i) + "]";
				glUniformMatrix4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), smi.c_str()), 6, false, lightMatrices[i].values);
			}
		}
		glUniform1f(glGetUniformLocation(GetCurrentShader()->GetProgram(), "farPlane"), 30000.0f);
	}

	if (IsShadowDebuggingModeOn())
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "shadowDebugMode"), true);
	else
		glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "shadowDebugMode"), false);
}

void Renderer::SetLighting(SceneNode * n)
{
	if (n->GetMesh() == heightMap || n->GetMesh() == treeMesh || n->GetMesh() == rockMesh || n->GetMesh() == earthMesh || 
		n->GetMesh() == reflectiveSphereMesh)
	{
		if (IsMovingLightModeOn())
			SetShaderLight(*movingLight);
		else
			SetShaderLight(*mainLight);
	}
}

void Renderer::SetHeightMapBufferTexture(SceneNode * n)
{
	if (n->GetMesh() == heightMap)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetMesh()->GetTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, n->GetMesh()->GetBumpMap());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, snowBufferColourTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, rainBufferColourTexture);
	}
}

void Renderer::SwitchScenesRendering()
{
	int sceneCounter = GetSceneManager()->GetCurrentScene();

	switch (sceneCounter)
	{
	case FIRST_SCENE:
		if (!fboInUse)
			RenderWeather();
		break;
	case SECOND_SCENE:
		break;
	case FINAL_SCENE:
		break;
	default:
		PrintToConsole("Something went wrong in the Renderer when switching the scenes while Rendering.", 1);
		exit(1);
	}
	BuildNodeLists(rootNodes[sceneCounter]);
}

void Renderer::SwitchScenesUpdating()
{
	int sceneCounter = GetSceneManager()->GetCurrentScene();

	switch (sceneCounter)
	{
	case FIRST_SCENE:
		UpdateWeather();
		UpdateLights();
		if (IsMovingLightModeOn())
			UpdateMovingLight(movingLight, lightMovementDelta, false);
		break;
	case SECOND_SCENE:
		UpdatePlanets();
		UpdateLights();
		if (IsMovingLightModeOn())
			UpdateMovingLight(movingLight, 0.0f, true);
		else
			GetSunNode()->SetTransform(Matrix4::Translation(SUN_STARTING_POS) * Matrix4::Rotation(0.0f, Vector3(0, 0, 1)));
		break;
	case FINAL_SCENE:
		break;
	default:
		PrintToConsole("Something went wrong in the Renderer when switching the scenes while Updating.", 1);
		exit(1);
	}
	rootNodes[sceneCounter]->Update(msec);
}

void Renderer::SetShaderParticleSize(float f)
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::DrawSkybox()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	SetCurrentShader(skyboxShader);
	modelMatrix.ToIdentity();

	glActiveTexture(GL_TEXTURE0);
	if (GetSceneManager()->GetCurrentScene() == FIRST_SCENE)
	{
		// Choose the appropriate cubeMap texture for the weather type
		if (renderSnow || renderRain)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapCloudyTexture);
			skyMesh->SetTexture(cubeMapCloudyTexture);
		}
		else
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapSunnyTexture);
			skyMesh->SetTexture(cubeMapSunnyTexture);
		}
	}
	else if (GetSceneManager()->GetCurrentScene() == SECOND_SCENE || GetSceneManager()->GetCurrentScene() == FINAL_SCENE)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapSpaceTexture);
		skyMesh->SetTexture(cubeMapSpaceTexture);
	}
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)&camera->GetPosition());
	UpdateShaderMatrices();

	skyMesh->Draw();

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(direction, direction));

		if (from->GetColour().w < 1.0f)
			transparentNodeList.push_back(from);
		else
			nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
		BuildNodeLists((*i));
}

void Renderer::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i)
		DrawNode((*i));

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawNode((*i));
		glDisable(GL_BLEND);
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}
