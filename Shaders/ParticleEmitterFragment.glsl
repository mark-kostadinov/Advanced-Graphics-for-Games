#version 330 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void)	{
	FragColor = IN.colour;
	if(useTexture > 0) 
	{
		FragColor	*= texture(diffuseTex, IN.texCoord);
	}
}