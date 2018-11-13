#version 440 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec4 viewSpace;
} OUT;
 
void main()
{
	OUT.position = (modelMatrix * vec4(position, 1.0)).xyz;
	OUT.normal = normalize(mat3(modelMatrix) * IN.normal);
	OUT.texCoord = texCoord;
	OUT.viewSpace = viewMatrix * modelMatrix * vec4(position, 1.0);
	gl_Position = projMatrix * viewSpace;
}