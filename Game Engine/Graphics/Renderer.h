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
	void	UpdateCameraPosition();

	Window*			GetWindow() const							{ return w; }
	void			SetWindow(Window* w)						{ this->w = w; }
	SceneManager*	GetSceneManager() const						{ return sceneManager; }
	void			SetSceneManager(SceneManager* sceneManager)	{ this->sceneManager = sceneManager; }

	Camera*		GetCamera() const						{ return camera; }
	void		SetCamera(Camera* c)					{ camera = c; }
	Light*		GetMainLight() const					{ return mainLight; }
	void		SetMainLight(Light* l)					{ mainLight = l; }
	Light*		GetMovingLight() const					{ return movingLight; }
	void		SetMovingLight(Light* l)				{ movingLight = l; }
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
	// SECOND_SCENE
	SceneNode*	GetEarthNode() const					{ return earthNode; }
	void		SetEarthNode(SceneNode* n)				{ earthNode = n; }
	SceneNode*	GetReflectiveSphereNode() const			{ return reflectiveSphereNode; }
	void		SetReflectiveSphereNode(SceneNode* n)	{ reflectiveSphereNode = n; }
	SceneNode*	GetSunNode() const						{ return sunNode; }
	void		SetSunNode(SceneNode* n)				{ sunNode = n; }
	SceneNode*	GetMoonNode() const						{ return moonNode; }
	void		SetMoonNode(SceneNode* n)				{ moonNode = n; }

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
	// SECOND_SCENE
	OBJMesh*	GetEarthMesh() const					{ return earthMesh; }
	void		SetEarthMesh(OBJMesh* m)				{ earthMesh = m; }
	OBJMesh*	GetReflectiveMesh() const				{ return reflectiveSphereMesh; }
	void		SetReflectiveMesh(OBJMesh* m)			{ reflectiveSphereMesh = m; }
	OBJMesh*	GetSunMesh() const						{ return sunMesh; }
	void		SetSunMesh(OBJMesh* m)					{ sunMesh = m; }
	OBJMesh*	GetMoonMesh() const						{ return moonMesh; }
	void		SetMoonMesh(OBJMesh* m)					{ moonMesh = m; }
	
	// FIRST_SCENE
	GLuint		GetCubeMapSunnyTexture() const			{ return cubeMapSunnyTexture; }
	void		SetCubeMapSunnyTexture(GLuint t)		{ cubeMapSunnyTexture = t; }
	GLuint		GetCubeMapCloudyTexture() const			{ return cubeMapCloudyTexture; }
	void		SetCubeMapCloudyTexture(GLuint t)		{ cubeMapCloudyTexture = t; }
	GLuint		GetHeightMapTexture() const				{ return heightMapTexture; }
	void		SetHeightMapTexture(GLuint t)			{ heightMapTexture = t; }
	GLuint		GetHeightMapBumpTexture() const			{ return heightMapBumpTexture; }
	void		SetHeightMapBumpTexture(GLuint t)		{ heightMapBumpTexture = t; }
	// SECOND_SCENE
	GLuint		GetShadowTexture() const				{ return shadowTexture; }
	void		SetShadowTexture(GLuint t)				{ shadowTexture = t; }
	GLuint		GetCubeMapSpaceTexture() const			{ return cubeMapSpaceTexture; }
	void		SetCubeMapSpaceTexture(GLuint t)		{ cubeMapSpaceTexture = t; }
	GLuint		GetEarthTexture() const					{ return earthTexture; }
	void		SetEarthTexture(GLuint t)				{ earthTexture = t; }
	GLuint		GetOmniDirShadowCubeMapTexture() const	{ return omniShadowCubeMapTexture; }
	void		SetOmniDirShadowCubeMapTexture(GLuint t) { omniShadowCubeMapTexture = t; }
	GLuint		GetSunTexture() const					{ return sunTexture; }
	void		SetSunTexture(GLuint t)					{ sunTexture = t; }
	GLuint		GetMoonTexture() const					{ return moonTexture; }
	void		SetMoonTexture(GLuint t)				{ moonTexture = t; }

	// FIRST_SCENE
	Shader*		GetDefaultShader() const				{ return defaultShader; }
	void		SetDefaultShader(Shader* s)				{ defaultShader = s; }
	Shader*		GetSceneNodeShader() const				{ return sceneNodeShader; }
	void		SetSceneNodeShader(Shader* s)			{ sceneNodeShader = s; }
	Shader*		GetSceneObjectShader() const			{ return sceneObjectShader; }
	void		SetSceneObjectShader(Shader* s)			{ sceneObjectShader = s; }
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
	Shader*		GetFontShader() const					{ return fontShader; }
	void		SetFontShader(Shader* s)				{ fontShader = s; }
	// SECOND_SCENE
	Shader*		GetShadowShader() const					{ return shadowShader; }
	void		SetShadowShader(Shader* s)				{ shadowShader = s; }
	Shader*		GetOmniShadowShader() const				{ return omniShadowShader; }
	void		SetOmniShadowShader(Shader* s)			{ omniShadowShader = s; }
	Shader*		GetSceneObjectOmniShader() const		{ return sceneObjectOmniShader; }
	void		SetSceneObjectOmniShader(Shader* s)		{ sceneObjectOmniShader = s; }
	Shader*		GetReflectionShader() const				{ return reflectionShader; }
	void		SetReflectionShader(Shader* s)			{ reflectionShader = s; }

	// Snow and rain rendering booleans
	void		SetRenderSnow(bool b)	{ renderSnow = b; }
	void		SetRenderRain(bool b)	{ renderRain = b; }
	void		SetSnowOnce(bool b)		{ snowOnce = b; }
	void		SetRainOnce(bool b)		{ rainOnce = b; }

	// Toggling different modes
	bool		IsShadowDebuggingModeOn() const			{ return shadowDebuggingMode; }
	void		SetShadowDebuggingMode(bool b)			{ shadowDebuggingMode = b; }
	bool		IsMovingLightModeOn() const				{ return movingLightMode; }
	void		SetMovingLightMode(bool b)				{ movingLightMode = b; }

	// Buffer for the data that comes from the performance timer
	string textBuffer;

	bool isFirstMiniScene = true;
	bool isSecondMiniScene = false;

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
	void	DrawShadowScene();
	void	ApplyPostProcessEffects();
	void	ResetPPFX();
	void	DrawOmniDirShadowScene();
	void	DrawShadows();
	void	RenderSceneFBO();

	void	SetShaderParticleSize(float f);
	void	DrawSkybox();
	void	GenerateFBOs();
	void	GenerateDepthTextureFBO(GLuint &texture);
	void	GenerateColourTextureFBO(GLuint &texture);
	void	AttachTexturesFBO(GLuint &fbo, GLuint &colourTexture, GLuint &depthTexture);
	void	AttachDepthTextureFBO(GLuint &fbo, GLuint &depthTexture);
	void	DrawParticleCollisionFBO();

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

	void	UpdateLights();
	void	UpdateMovingLight(Light* l, float delta, bool orbit = NULL);
	void	UpdatePlanets();

	void	RenderText(const string &text);
	void	SplitText(vector<string> &v, const string &text);

	// Draws the passed in line of text, at the passed in position. 
	// Can have an optional font size, and whether to draw it in orthographic or perspective mode.
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	Window*				w;
	SceneManager*		sceneManager;

	Camera*				camera;
	Light*				mainLight;
	Light*				movingLight;

	// FIRST_SCENE
	SceneNode*			rootNodes[scenesCount];
	SceneNode*			originCubeNode;
	SceneNode*			heightMapNode;
	SceneNode*			snowEmitterNode;
	SceneNode*			rainEmitterNode;
	vector<SceneNode*>	treeNodes;
	vector<SceneNode*>	rockNodes;
	// SECOND_SCENE
	SceneNode*			earthNode;
	SceneNode*			reflectiveSphereNode;
	SceneNode*			sunNode;
	SceneNode*			moonNode;

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
	OBJMesh*		earthMesh;
	OBJMesh*		reflectiveSphereMesh;
	OBJMesh*		sunMesh;
	OBJMesh*		moonMesh;

	// FIRST_SCENE
	GLuint		cubeMapSunnyTexture;
	GLuint		cubeMapCloudyTexture;
	GLuint		heightMapTexture;
	GLuint		heightMapBumpTexture;
	// SECOND_SCENE
	GLuint		cubeMapSpaceTexture;
	GLuint		earthTexture;
	GLuint		sunTexture;
	GLuint		moonTexture;

	// FIRST_SCENE
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
	// SECOND_SCENE
	GLuint		shadowFBO;
	GLuint		shadowTexture;
	GLuint		omniShadowFBO;
	GLuint		omniShadowCubeMapTexture;

	// FIRST_SCENE
	Shader*		defaultShader;
	Shader*		sceneNodeShader;
	Shader*		sceneObjectShader;
	Shader*		skyboxShader;
	Shader*		heightMapShader;
	Shader*		snowParticleShader;
	Shader*		rainParticleShader;
	Shader*		snowCollisionShader;
	Shader*		rainCollisionShader;
	Shader*		postProcessingShader;
	Shader*		fontShader;
	// SECOND_SCENE
	Shader*		shadowShader;
	Shader*		omniShadowShader;
	Shader*		sceneObjectOmniShader;
	Shader*		reflectionShader;

	bool renderSnow = false;
	bool renderRain = false;
	bool snowOnce = false;
	bool rainOnce = false;

	float msec = 0.0f;
	float alphaFade = 1.0f;
	float blurFactor = 1.0f;
	float planetRotationAngle = 0.1f;
	bool fboInUse = false;
	bool castShadows = false;
	bool castOmniShadows = false;
	Matrix4 lightMatrices[6];

	bool shadowDebuggingMode = false;
	bool movingLightMode = false;

	Frustum		frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};