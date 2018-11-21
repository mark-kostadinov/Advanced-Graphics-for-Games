#version 440 core

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;
uniform int isFadingOut;
uniform float alphaFade;

in Vertex {
  vec2 texCoord;
} IN;

out vec4 FragColour;

const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12);

void main(void) 
{
	vec2 values[5];
	vec4 colourToFade;

	// Blurring
	if(isVertical == 1) 
	{
		values = vec2[](vec2(0.0, -pixelSize.y * 2),
		vec2(0.0, -pixelSize.y * 1), vec2(0.0, 0.0),
		vec2(0.0, pixelSize.y * 1), vec2(0.0, pixelSize.y * 2));
	}
	else 
	{
		values = vec2[](vec2(-pixelSize.x * 2, 0.0),
		vec2(-pixelSize.x * 1, 0.0), vec2(0.0, 0.0),
		vec2(pixelSize.x * 1, 0.0), vec2(pixelSize.x * 2, 0.0));
	}

	for (int i = 0; i < 5; i++) 
	{
		vec4 diffuse = texture(diffuseTex, IN.texCoord.xy + values[i]);
		FragColour += diffuse * weights[i];
	}
	
	// Alpha fading
	if (isFadingOut == 1) {
		colourToFade = FragColour;
		FragColour = vec4(colourToFade.rgb, alphaFade);
	}
}
