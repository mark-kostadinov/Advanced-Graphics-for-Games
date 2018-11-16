#version 440 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColour;
 
void main()
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
		
	if (diffuse.a == 0)
		discard;
	else
		FragColour = vec4(1.0);
}