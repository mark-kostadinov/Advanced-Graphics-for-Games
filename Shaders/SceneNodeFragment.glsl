#version 440 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 FragColor;

void main(void)	{
	FragColor = IN.colour;
	if(useTexture > 0) {
		vec4 diffuse = texture(diffuseTex, IN.texCoord);
		
		if (diffuse.a == 0)
			discard;
		else
			FragColor *= diffuse;
	}
}