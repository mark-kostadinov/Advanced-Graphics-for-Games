#version 330 core

uniform sampler2D snowBufferColourTexture;
uniform sampler2D rainBufferColourTexture;
uniform bool renderSnow;
uniform bool renderRain;
uniform float terrainSize;
uniform float terrainGridSize;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

in vec3 position;
in vec3 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;

void main(void)	{
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 newPos = position;
	
	if (renderSnow) 
	{
		// Sample snowBufferColourTexture to add to the y axis
		vec4 snowAccum = texture(snowBufferColourTexture, texCoord / (terrainSize / terrainGridSize));
		
		if (snowAccum.y >= 0.9) {
			newPos.y += snowAccum.x * 0.5f;
		}
	}
	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = normalize(normalMatrix * normalize(tangent));
	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;
	// The larger the scale of the scene, the smaller the normals need to be
	OUT.shadowProj = (shadowMatrix * vec4(position + (normal * 0.0075), 1));	

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(newPos, 1.0);
}