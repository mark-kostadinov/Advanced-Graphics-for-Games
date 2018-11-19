// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616

#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(Renderer * renderer)
{
	// Capture the maximum possible anisotropic filtering level
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestSupportedAnisotropyLevel);

	SetRenderer(renderer);
	SetInitialised(true);
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::LoadResources()
{
	LoadShaders();
	LoadTextures();
	SetMeshes();
	SetSceneNodes();

	SetLoadedAssets(true);
}

void ResourceManager::EnableAnisotropicFiltering(GLuint texture, GLfloat anisotropyLevel)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceManager::LoadShaders()
{
	// FIRST_SCENE
	GetRenderer()->SetDefaultShader(new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl"));
	GetRenderer()->SetSceneNodeShader(new Shader(SHADERDIR"SceneNodeVertex.glsl", SHADERDIR"SceneNodeFragment.glsl"));
	GetRenderer()->SetSceneObjectShader(new Shader(SHADERDIR"SceneObjectVertex.glsl", SHADERDIR"SceneObjectFragment.glsl"));
	GetRenderer()->SetSkyboxShader(new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl"));
	GetRenderer()->SetHeightMapShader(new Shader(SHADERDIR"HeightMapVertex.glsl", SHADERDIR"HeightMapFragment.glsl"));/// , SHADERDIR"HeightMapTesselationControl.glsl", SHADERDIR"HeightMapTesselationEvaluation.glsl"));
	GetRenderer()->SetSnowParticleShader(new Shader(SHADERDIR"ParticleEmitterVertex.glsl", SHADERDIR"ParticleEmitterFragment.glsl", "", "", SHADERDIR"ParticleEmitterGeometry.glsl"));
	GetRenderer()->SetRainParticleShader(new Shader(SHADERDIR"ParticleEmitterVertex.glsl", SHADERDIR"ParticleEmitterFragment.glsl", "", "", SHADERDIR"ParticleEmitterGeometry.glsl"));
	GetRenderer()->SetSnowCollisionShader(new Shader(SHADERDIR"SnowCollisionVertex.glsl", SHADERDIR"SnowCollisionFragment.glsl"));
	GetRenderer()->SetRainCollisionShader(new Shader(SHADERDIR"RainCollisionVertex.glsl", SHADERDIR"RainCollisionFragment.glsl"));
	GetRenderer()->SetPostProcessingShader(new Shader(SHADERDIR"PostProcessingVertex.glsl", SHADERDIR"PostProcessingFragment.glsl"));
	//GetRenderer()->SetFogShader(new Shader(SHADERDIR"FogVertex.glsl", SHADERDIR"FogFragment.glsl"));
	GetRenderer()->SetFontShader(new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl"));
	// SECOND_SCENE
	GetRenderer()->SetShadowShader(new Shader(SHADERDIR"ShadowVertex.glsl", SHADERDIR"ShadowFragment.glsl"));
	GetRenderer()->SetReflectionShader(new Shader(SHADERDIR"ReflectionVertex.glsl", SHADERDIR"ReflectionFragment.glsl"));
	GetRenderer()->SetReflectiveTextureShader(new Shader(SHADERDIR"ReflectiveTextureVertex.glsl", SHADERDIR"ReflectiveTextureFragment.glsl"));

	if (!GetRenderer()->GetDefaultShader()->LinkProgram() || !GetRenderer()->GetSceneNodeShader()->LinkProgram() ||
		!GetRenderer()->GetSceneObjectShader()->LinkProgram() || !GetRenderer()->GetSkyboxShader()->LinkProgram() || 
		!GetRenderer()->GetHeightMapShader()->LinkProgram() || !GetRenderer()->GetSnowParticleShader()->LinkProgram() || 
		!GetRenderer()->GetRainParticleShader()->LinkProgram() || !GetRenderer()->GetSnowCollisionShader()->LinkProgram() || 
		!GetRenderer()->GetRainCollisionShader()->LinkProgram() || !GetRenderer()->GetPostProcessingShader()->LinkProgram() || 
		!GetRenderer()->GetFontShader()->LinkProgram() || !GetRenderer()->GetShadowShader()->LinkProgram() || 
		!GetRenderer()->GetReflectionShader()->LinkProgram() || !GetRenderer()->GetReflectiveTextureShader()->LinkProgram()) // !GetRenderer()->GetFogShader()->LinkProgram()
	{
		PrintToConsole("Could not link one of the shaders!", 1);
		return;
	}
}

void ResourceManager::LoadTextures()
{
	// FIRST_SCENE
#ifdef DEBUG
	GetRenderer()->SetHeightMapTexture(SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	GetRenderer()->SetHeightMapBumpTexture(SOIL_load_OGL_texture(TEXTUREDIR"Grass_Norm.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
#endif // DEBUG
#ifndef DEBUG
	GetRenderer()->SetHeightMapTexture(SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	EnableAnisotropicFiltering(GetRenderer()->GetHeightMapTexture(), largestSupportedAnisotropyLevel);
	GetRenderer()->SetHeightMapBumpTexture(SOIL_load_OGL_texture(TEXTUREDIR"Grass_Norm.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	EnableAnisotropicFiltering(GetRenderer()->GetHeightMapBumpTexture(), largestSupportedAnisotropyLevel);
#endif // DEBUG
	GetRenderer()->SetCubeMapSunnyTexture(SOIL_load_OGL_cubemap(
		TEXTUREDIR"TropicalSunnyDay/Left.PNG", TEXTUREDIR"TropicalSunnyDay/Right.PNG", TEXTUREDIR"TropicalSunnyDay/Up.PNG",
		TEXTUREDIR"TropicalSunnyDay/Down.PNG", TEXTUREDIR"TropicalSunnyDay/Front.PNG", TEXTUREDIR"TropicalSunnyDay/Back.PNG",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0));
	GetRenderer()->SetCubeMapCloudyTexture(SOIL_load_OGL_cubemap(
		TEXTUREDIR"CloudyLightRays/Left.PNG", TEXTUREDIR"CloudyLightRays/Right.PNG", TEXTUREDIR"CloudyLightRays/Up.PNG",
		TEXTUREDIR"CloudyLightRays/Down.PNG", TEXTUREDIR"CloudyLightRays/Front.PNG", TEXTUREDIR"CloudyLightRays/Back.PNG",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0));
	GetRenderer()->SetFont(new Font(SOIL_load_OGL_texture(
		TEXTUREDIR"tahoma.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16));

	// SECOND_SCENE
	GetRenderer()->SetCubeMapSpaceTexture(SOIL_load_OGL_cubemap(
		TEXTUREDIR"sor_cwd/cwd_lf.JPG", TEXTUREDIR"sor_cwd/cwd_rt.JPG", TEXTUREDIR"sor_cwd/cwd_up.JPG",
		TEXTUREDIR"sor_cwd/cwd_dn.JPG", TEXTUREDIR"sor_cwd/cwd_ft.JPG", TEXTUREDIR"sor_cwd/cwd_bk.JPG",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0));

	CheckTextureLoading();
}

void ResourceManager::CheckTextureLoading()
{
	// FIRST_SCENE
	if (!GetRenderer()->GetHeightMapTexture())
	{
		PrintToConsole("Could not load the heightmap texture!", 1);
		return;
	}
	GetRenderer()->SetTextureRepeating(GetRenderer()->GetHeightMapTexture(), true);

	if (!GetRenderer()->GetHeightMapBumpTexture())
	{
		PrintToConsole("Could not load the heightmap bumpMap texture!", 1);
		return;
	}
	GetRenderer()->SetTextureRepeating(GetRenderer()->GetHeightMapBumpTexture(), true);

	if (!GetRenderer()->GetCubeMapSunnyTexture())
	{
		PrintToConsole("Could not load the sunny cubeMap texture!", 1);
		return;
	}
	GetRenderer()->SetTextureRepeating(GetRenderer()->GetCubeMapSunnyTexture(), true);

	if (!GetRenderer()->GetCubeMapCloudyTexture())
	{
		PrintToConsole("Could not load the cloudy cubeMap texture!", 1);
		return;
	}
	GetRenderer()->SetTextureRepeating(GetRenderer()->GetCubeMapCloudyTexture(), true);

	if (!GetRenderer()->GetFont())
	{
		PrintToConsole("Could not load the text font!", 1);
		return;
	}

	// SECOND_SCENE
	if (!GetRenderer()->GetCubeMapSpaceTexture())
	{
		PrintToConsole("Could not load the space cubeMap texture!", 1);
		return;
	}
	GetRenderer()->SetTextureRepeating(GetRenderer()->GetCubeMapSpaceTexture(), true);
}

void ResourceManager::SetMeshes()
{
	// FIRST_SCENE
	GetRenderer()->SetVersatileMesh(Mesh::GenerateQuad());
	GetRenderer()->SetWeatherEffectsMesh(Mesh::GenerateQuad());
	GetRenderer()->SetOriginCubeMesh(Mesh::LoadMeshFile(MESHDIR"cube.asciimesh"));
	GetRenderer()->SetSkyMesh(Mesh::GenerateQuad());

	// Set the heightMap
	GetRenderer()->SetHeightMap(new HeightMap(TEXTUREDIR"terrain512.raw"));
	GetRenderer()->GetHeightMap()->SetTexture(GetRenderer()->GetHeightMapTexture());
	if (!GetRenderer()->GetHeightMap()->GetTexture())
	{
		PrintToConsole("Could not load the heightMap texture!", 1);
		return;
	}

	GetRenderer()->GetHeightMap()->SetBumpMap(GetRenderer()->GetHeightMapBumpTexture());
	if (!GetRenderer()->GetHeightMap()->GetBumpMap())
	{
		PrintToConsole("Could not load the heightMap bumpMap texture!", 1);
		return;
	}
	
	// Set the particle emitters
	GetRenderer()->SetSnowEmitter(new SnowEmitter());
	GetRenderer()->GetSnowEmitter()->SetEmitterPosition(PARTICLE_EMITTER_POSITION);
	GetRenderer()->SetRainEmitter(new RainEmitter());
	GetRenderer()->GetRainEmitter()->SetEmitterPosition(PARTICLE_EMITTER_POSITION);

	// Load the tree mesh
	GetRenderer()->SetTreeMesh(new OBJMesh());
	GetRenderer()->GetTreeMesh()->LoadOBJMesh(MESHDIR"Tree.obj");
	EnableAnisotropicFiltering(GetRenderer()->GetTreeMesh()->GetTexture(), largestSupportedAnisotropyLevel);

	// Load the rock mesh
	GetRenderer()->SetRockMesh(new OBJMesh());
	GetRenderer()->GetRockMesh()->LoadOBJMesh(MESHDIR"Rock.obj");
	EnableAnisotropicFiltering(GetRenderer()->GetRockMesh()->GetTexture(), largestSupportedAnisotropyLevel);

	// SECOND_SCENE
	GetRenderer()->SetTexturedSphereMesh(new OBJMesh());
	GetRenderer()->GetTexturedSphereMesh()->LoadOBJMesh(MESHDIR"texturedSphere.obj");
	GetRenderer()->GetTexturedSphereMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"earth.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); /// TODO: add a GLuint
	if (!GetRenderer()->GetTexturedSphereMesh()->GetTexture())
	{
		PrintToConsole("Could not load the textured sphere texture!", 1);
		return;
	}
	EnableAnisotropicFiltering(GetRenderer()->GetTexturedSphereMesh()->GetTexture(), largestSupportedAnisotropyLevel);

	GetRenderer()->SetReflectiveMesh(new OBJMesh());
	GetRenderer()->GetReflectiveMesh()->LoadOBJMesh(MESHDIR"reflectiveSphere.obj");
}

void ResourceManager::SetSceneNodes()
{
	// Some sensible values for node manipulation
	Vector3 origin = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 defaultModelScale = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 treeModelScale = Vector3(1000.0f, 1200.0f, 1000.0f);
	Vector3 rockModelScale = Vector3(10.0f, 10.0f, 10.0f);
	Vector4 defaultOpaqueColourVector = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 defaultTransparentColourVector = Vector4(1.0f, 1.0f, 1.0f, 0.999f);
	Matrix4 defaultRotationMatrix = Matrix4::Rotation(0.0f, Vector3(0.0f, 0.0f, 1.0f));

	// Start with a root for each scene
	for (int i = 0; i < scenesCount; i++)
		GetRenderer()->SetRootNode(i, new SceneNode());

	// FIRST_SCENE
#ifdef DEBUG
	GetRenderer()->SetOriginCubeNode(new SceneNode(GetRenderer()->GetOriginCubeMesh(), ORIGIN_CUBE_COLOUR, GetRenderer()->GetSceneNodeShader()));
	GetRenderer()->GetOriginCubeNode()->SetTransform(Matrix4::Translation(origin) * defaultRotationMatrix);
	GetRenderer()->GetOriginCubeNode()->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	GetRenderer()->GetOriginCubeNode()->SetBoundingRadius(100.0f);
	GetRenderer()->GetRootNode(FIRST_SCENE)->AddChild(GetRenderer()->GetOriginCubeNode());
#endif // DEBUG

	GetRenderer()->SetHeightMapNode(new SceneNode(GetRenderer()->GetHeightMap(), defaultOpaqueColourVector, GetRenderer()->GetHeightMapShader()));
	GetRenderer()->GetHeightMapNode()->SetTransform(Matrix4::Translation(origin) * defaultRotationMatrix);
	GetRenderer()->GetHeightMapNode()->SetModelScale(defaultModelScale);
	GetRenderer()->GetHeightMapNode()->SetBoundingRadius(farPlane);
	GetRenderer()->GetRootNode(FIRST_SCENE)->AddChild(GetRenderer()->GetHeightMapNode());

	GetRenderer()->SetSnowEmitterNode(new SceneNode(GetRenderer()->GetSnowEmitter(), defaultTransparentColourVector, GetRenderer()->GetSnowParticleShader()));
	GetRenderer()->GetSnowEmitterNode()->SetTransform(Matrix4::Translation(GetRenderer()->GetSnowEmitter()->GetEmitterPosition()) * defaultRotationMatrix);
	GetRenderer()->GetSnowEmitterNode()->SetModelScale(defaultModelScale);
	GetRenderer()->GetSnowEmitterNode()->SetBoundingRadius(farPlane);
	GetRenderer()->GetRootNode(FIRST_SCENE)->AddChild(GetRenderer()->GetSnowEmitterNode());

	GetRenderer()->SetRainEmitterNode(new SceneNode(GetRenderer()->GetRainEmitter(), defaultTransparentColourVector, GetRenderer()->GetRainParticleShader()));
	GetRenderer()->GetRainEmitterNode()->SetTransform(Matrix4::Translation(GetRenderer()->GetRainEmitter()->GetEmitterPosition()) * defaultRotationMatrix);
	GetRenderer()->GetRainEmitterNode()->SetModelScale(defaultModelScale);
	GetRenderer()->GetRainEmitterNode()->SetBoundingRadius(farPlane);
	GetRenderer()->GetRootNode(FIRST_SCENE)->AddChild(GetRenderer()->GetRainEmitterNode());

	for (int i = 0; i < numberOfTrees; i++)
	{
		GenerateObjects(GetRenderer()->GetTreeMesh(), defaultOpaqueColourVector, GetRenderer()->GetSceneObjectShader(), 200.0f, defaultRotationMatrix, treeModelScale, 10000.0f, GetRenderer()->GetTreeNodesVector(), GetRenderer()->GetRootNode(FIRST_SCENE));
	}

	for (int i = 0; i < numberOfRocks; i++)
	{
		float randSize = (float)(rand() % 10 + 1);
		GenerateObjects(GetRenderer()->GetRockMesh(), defaultOpaqueColourVector, GetRenderer()->GetSceneObjectShader(), 20.0f, defaultRotationMatrix, Vector3(randSize, randSize, randSize), 10000.0f, GetRenderer()->GetRockNodesVector(), 
			GetRenderer()->GetRootNode(FIRST_SCENE));
	}

	// SECOND_SCENE
	GetRenderer()->SetTexturedSphereNode(new SceneNode(GetRenderer()->GetTexturedSphereMesh(), defaultOpaqueColourVector, GetRenderer()->GetSceneObjectShader()));
	GetRenderer()->GetTexturedSphereNode()->SetTransform(Matrix4::Translation(Vector3(3000.0f, 1000.0f, 3000.0f)) * defaultRotationMatrix);
	GetRenderer()->GetTexturedSphereNode()->SetModelScale(Vector3(50.0f, 50.0f, 50.0f));
	GetRenderer()->GetTexturedSphereNode()->SetBoundingRadius(5000.0f);
	GetRenderer()->GetRootNode(SECOND_SCENE)->AddChild(GetRenderer()->GetTexturedSphereNode());

	GetRenderer()->SetReflectiveSphereNode(new SceneNode(GetRenderer()->GetReflectiveMesh(), defaultTransparentColourVector, GetRenderer()->GetReflectionShader()));
	GetRenderer()->GetReflectiveSphereNode()->SetTransform(Matrix4::Translation(Vector3(0.0f, 1000.0f, 0.0f)) * defaultRotationMatrix);
	GetRenderer()->GetReflectiveSphereNode()->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
	GetRenderer()->GetReflectiveSphereNode()->SetBoundingRadius(5000.0f);
	GetRenderer()->GetRootNode(SECOND_SCENE)->AddChild(GetRenderer()->GetReflectiveSphereNode());

	// FINAL_SCENE
}

void ResourceManager::GenerateObjects(OBJMesh * mesh, Vector4 colourVector, Shader * shader, float heightDisplacement, Matrix4 rotationMatrix,	Vector3 modelScale, float boundingRadius, vector<SceneNode*> parentVector, SceneNode * rootNode)
{
	SceneNode* tempNode = new SceneNode(mesh, colourVector, shader);
	Vector2 randomPosition = GenerateRandomPosition((int)(RAW_HEIGHT * HEIGHTMAP_X), (int)(RAW_WIDTH * HEIGHTMAP_Z));
	tempNode->SetTransform(Matrix4::Translation(
		Vector3(randomPosition.x, GetRenderer()->GetHeightMap()->GetTerrainHeight(randomPosition.x, randomPosition.y) - heightDisplacement, randomPosition.y)) * rotationMatrix);
	tempNode->SetModelScale(modelScale);
	tempNode->SetBoundingRadius(boundingRadius);
	rootNode->AddChild(tempNode);
	parentVector.push_back(tempNode);
}
