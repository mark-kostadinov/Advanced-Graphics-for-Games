#version 440 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;
 
void main()
{
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	gl_Position = modelMatrix * vec4(position, 1.0);
}