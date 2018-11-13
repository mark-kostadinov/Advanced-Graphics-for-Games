#pragma once

#include <algorithm>	
#include <mutex>
#include "../Game Engine/definitions.h"
#include "../../nclgl/OGLRenderer.h"
#include "../Game Engine/SceneManager.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/TextMesh.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/OBJMesh.h"
#include "SnowEmitter.h"
#include "RainEmitter.h"

constexpr float nearPlane = 1.0f;
constexpr float farPlane = 60000.0f;
constexpr float fov = 60.0f;

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent, SceneManager * sceneManager);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

	void	ResetWeatherConditions();

	Window*			GetWindow() const							{ return w; }
	void			SetWindow(Window* w)						{ this->w = w; }
	SceneManager*	GetSceneManager() const						{ return sceneManager; }
	void			SetSceneManager(SceneManager* sceneManager)	{ this->sceneManager = sceneManager; }

	Camera*		GetCamera() const						{ return camera; }
	void		SetCamera(Camera* c)					{ camera = c; }
	Light*		GetMainLight() const					{ return mainLight; }
	void		SetMainLight(Light* l)					{ mainLight = l; }
	vector<Light*> GetMovingLightsVector() const		{ return movingLightsVector; }
	Font*		GetFont() const							{ return dataFont; }
	void		SetFont(Font* f)						{ dataFont = f; }

	SceneNode*	GetRootNode(int i) const				{ return rootNodes[i]; }
	void		SetRootNode(int i, SceneNode* n)		{ rootNodes[i] = n; }
	SceneNode*	GetOriginCubeNode() const				{ return originCubeNode; }
	void		SetOriginCubeNode(SceneNode* n)			{ originCubeNode = n; }
	SceneNode*	GetHeightMapNode() const				{ return heightMapNode; }
	void		SetHeightMapNode(SceneNode* n)			{ heightMapNode = n; }
	SceneNode*	GetSnowEmitterNode() const				{ return snowEmitterNode; }
	void		SetSnowEmitterNode(SceneNode* n)		{ snowEmitterNode = n; }
	SceneNode*	GetRainEmitterNode() const				{ return rainEmitterNode; }
	void		SetRainEmitterNode(SceneNode* n)		{ rainEmitterNode = n; }
	vector<SceneNode*> GetTreeNodesVector() const		{ return treeNodes; }
	vector<SceneNode*> GetRockNodesVector() const		{ return rockNodes; }
	SceneNode*	GetCubeNode() const						{ return cubeNode; }
	void		SetCubeNode(SceneNode* n)				{ cubeNode = n; }

	HeightMap*	GetHeightMap() const					{ return heightMap; }
	void		SetHeightMap(HeightMap* h)				{ heightMap = h; }
	SnowEmitter* GetSnowEmitter() const					{ return snowEmitter; }
	void		SetSnowEmitter(SnowEmitter* s)			{ snowEmitter = s; }
	RainEmitter* GetRainEmitter() const					{ return rainEmitter; }
	void		SetRainEmitter(RainEmitter* r)			{ rainEmitter = r; }
	Mesh*		GetWeatherEffectsMesh() const			{ return weatherEffectsMesh; }
	void		SetWeatherEffectsMesh(Mesh* m)			{ weatherEffectsMesh = m; }
	Mesh*		GetVersatileMesh() const				{ return versatileMesh; }
	void		SetVersatileMesh(Mesh* m)				{ versatileMesh = m; }
	Mesh*		GetOriginCubeMesh() const				{ return originCubeMesh; }
	void		SetOriginCubeMesh(Mesh* m)				{ originCubeMesh = m; }
	Mesh*		GetSkyMesh() const						{ return skyMesh; }
	void		SetSkyMesh(Mesh* m)						{ skyMesh = m; }
	OBJMesh*	GetTreeMesh() const						{ return treeMesh; }
	void		SetTreeMesh(OBJMesh* m)					{ treeMesh = m; }
	OBJMesh*	GetRockMesh() const						{ return rockMesh; }
	void		SetRockMesh(OBJMesh* m)					{ rockMesh = m; }
	Mesh*		GetCubeMesh() const						{ return cubeMesh; }
	void		SetCubeMesh(Mesh* m)					{ cubeMesh = m; }
	
	GLuint		GetCubeMapSunnyTexture() const			{ return cubeMapSunnyTexture; }
	void		SetCubeMapSunnyTexture(GLuint t)		{ cubeMapSunnyTexture = t; }
	GLuint		GetCubeMapCloudyTexture() const			{ return cubeMapCloudyTexture; }
	void		SetCubeMapCloudyTexture(GLuint t)		{ cubeMapCloudyTexture = t; }
	GLuint		GetHeightMapTexture() const				{ return heightMapTexture; }
	void		SetHeightMapTexture(GLuint t)			{ heightMapTexture = t; }
	GLuint		GetHeightMapBumpTexture() const			{ return heightMapBumpTexture; }
	void		SetHeightMapBumpTexture(GLuint t)		{ heightMapBumpTexture = t; }
	GLuint		GetCubeTexture() const					{ return cubeTexture; }
	void		SetCubeTexture(GLuint t)				{ cubeTexture = t; }

	Shader*		GetDefaultShader() const				{ return defaultShader; }
	void		SetDefaultShader(Shader* s)				{ defaultShader = s; }
	Shader*		GetSceneNodeShader() const				{ return sceneNodeShader; }
	void		SetSceneNodeShader(Shader* s)			{ sceneNodeShader = s; }
	Shader*		GetSkyboxShader() const					{ return skyboxShader; }
	void		SetSkyboxShader(Shader* s)				{ skyboxShader = s; }
	Shader*		GetHeightMapShader() const				{ return heightMapShader; }
	void		SetHeightMapShader(Shader* s)			{ heightMapShader = s; }
	Shader*		GetSnowParticleShader() const			{ return snowParticleShader; }
	void		SetSnowParticleShader(Shader* s)		{ snowParticleShader = s; }
	Shader*		GetRainParticleShader() const			{ return rainParticleShader; }
	void		SetRainParticleShader(Shader* s)		{ rainParticleShader = s; }
	Shader*		GetSnowCollisionShader() const			{ return snowCollisionShader; }
	void		SetSnowCollisionShader(Shader* s)		{ snowCollisionShader = s; }
	Shader*		GetRainCollisionShader() const			{ return rainCollisionShader; }
	void		SetRainCollisionShader(Shader* s)		{ rainCollisionShader = s; }
	Shader*		GetPostProcessingShader() const			{ return postProcessingShader; }
	void		SetPostProcessingShader(Shader* s)		{ postProcessingShader = s; }
	Shader*		GetSceneObjectShader() const			{ return sceneObjectShader; }
	void		SetSceneObjectShader(Shader* s)			{ sceneObjectShader = s; }
	Shader*		GetFogShader() const					{ return fogShader; }
	void		SetFogShader(Shader* s)					{ fogShader = s; }
	Shader*		GetFontShader() const					{ return fontShader; }
	void		SetFontShader(Shader* s)				{ fontShader = s; }

	// Snow and rain rendering booleans
	void		SetRenderSnow(bool b)	{ renderSnow = b; }
	void		SetRenderRain(bool b)	{ renderRain = b; }
	void		SetSnowOnce(bool b)		{ snowOnce = b; }
	void		SetRainOnce(bool b)		{ rainOnce = b; }

	// Buffer for the data that comes from the performance timer
	string textBuffer;

protected:
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(SceneNode* n);
	void	SetUniforms(SceneNode* n);
	void	SetLighting(SceneNode* n);
	void	SetHeightMapBufferTexture(SceneNode* n);
	void	SwitchScenesRendering();
	void	SwitchScenesUpdating();
	void	DrawScene();
	void	ApplyPostProcessEffects();

	void	SetShaderParticleSize(float f);
	void	DrawSkybox();
	void	GenerateFBOs();
	void	GenerateDepthTextureFBO(GLuint &texture);
	void	GenerateColourTextureFBO(GLuint &texture);
	void	AttachTexturesFBO(GLuint &fbo, GLuint &colourTexture, GLuint &depthTexture);
	void	DrawParticleCollisionFBO();

	void	DrawFog();
	void	DrawPostProcess();
	void	RenderWeather();
	void	UpdateWeather();
	void	UpdateSnow();
	void	UpdateRain();
	void	ClearAccumulation();

	void	ResetMVP();
	void	SwitchToOrtho();
	void	DeleteObjects();
	void	DeleteShaders();
	void	DeleteTextures();

	void	GenerateMultipleLights(int count);
	void	UpdateMovingLights(vector<Light*> lightsVector);
	void	MoveLight(Light* l, float delta);

	void	RenderText(const string &text);
	void	SplitText(vector<string> &v, const string &text);

	// Draws the passed in line of text, at the passed in position. 
	// Can have an optional font size, and whether to draw it in orthographic or perspective mode.
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	Window*				w;
	SceneManager*		sceneManager;

	Camera*				camera;
	Light*				mainLight;
	vector<Light*>		movingLightsVector;

	// FIRST_SCENE
	SceneNode*			rootNodes[scenesCount];
	SceneNode*			originCubeNode;
	SceneNode*			heightMapNode;
	SceneNode*			snowEmitterNode;
	SceneNode*			rainEmitterNode;
	vector<SceneNode*>	treeNodes;
	vector<SceneNode*>	rockNodes;
	// SECOND_SCENE
	SceneNode*			cubeNode;

	// FIRST_SCENE
	Font*			dataFont;
	HeightMap*		heightMap;
	SnowEmitter*	snowEmitter;
	RainEmitter*	rainEmitter;
	Mesh*			versatileMesh;
	Mesh*			weatherEffectsMesh;
	Mesh*			originCubeMesh;
	Mesh*			skyMesh;
	OBJMesh*		treeMesh;
	OBJMesh*		rockMesh;
	// SECOND_SCENE
	Mesh*			cubeMesh;

	// FIRST_SCENE
	GLuint		cubeMapSunnyTexture;
	GLuint		cubeMapCloudyTexture;
	GLuint		heightMapTexture;
	GLuint		heightMapBumpTexture;
	// SECOND_SCENE
	GLuint		cubeTexture;

	GLuint		snowCollisionFBO;
	GLuint		snowBufferColourTexture;
	GLuint		snowBufferDepthTexture;
	GLuint		rainCollisionFBO;
	GLuint		rainBufferColourTexture;
	GLuint		rainBufferDepthTexture;
	GLuint		bufferFBO;
	GLuint		bufferColourTexture[2];
	GLuint		bufferDepthTexture;
	GLuint		postProcessingFBO;
	GLuint		postProcessingColourTexture;
	GLuint		postProcessingDepthTexture;

	Shader*		sceneNodeShader;
	Shader*		defaultShader;
	Shader*		skyboxShader;
	Shader*		heightMapShader;
	Shader*		snowParticleShader;
	Shader*		rainParticleShader;
	Shader*		snowCollisionShader;
	Shader*		rainCollisionShader;
	Shader*		postProcessingShader;
	Shader*		sceneObjectShader;
	Shader*		fogShader;
	Shader*		fontShader;

	bool renderSnow = false;
	bool renderRain = false;
	bool snowOnce = false;
	bool rainOnce = false;

	float msec = 0.0f;
	float alphaFade = 1.0f;
	float blurFactor = 1.0f;
	bool fboInUse = false;

	Frustum		frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};