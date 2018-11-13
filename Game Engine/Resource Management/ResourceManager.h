// Code written for CSC3224 by Mark Kostadinov, Student Number 150368616
//
// This class handles the initial loading of all resources, such as shaders, textures, meshes and Scene nodes.

#pragma once

#include "../../nclgl/Window.h"
#include "../Graphics/Renderer.h"

class ResourceManager
{
public:
	ResourceManager(Renderer * renderer);
	~ResourceManager();

	void		LoadResources();

	Renderer*	GetRenderer() const			{ return r; }
	void		SetRenderer(Renderer* r)	{ this->r = r; }

	bool		HasInitialised() const { return init; }
	void		SetInitialised(bool b) { init = b; }
	bool		HasLoadedAssets() const { return loadedAssets; }
	void		SetLoadedAssets(bool b) { loadedAssets = b; }

	static void EnableAnisotropicFiltering(GLuint texture, GLfloat anisotropyLevel);

protected:
	void		LoadShaders();
	void		LoadTextures();
	void		CheckTextureLoading();
	void		SetMeshes();
	void		SetSceneNodes();
	void		GenerateObjects(OBJMesh* mesh, Vector4 colourVector, Shader* shader, float heightDisplacement, Matrix4 rotationMatrix,				Vector3 modelScale, float boundingRadius, vector<SceneNode*> parentVector, SceneNode* rootNode);

	Renderer * r;

	GLfloat largestSupportedAnisotropyLevel;
	bool init;
	bool loadedAssets;
};
