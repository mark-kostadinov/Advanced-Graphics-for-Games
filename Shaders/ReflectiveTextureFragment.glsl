#version 440 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 FragColour;

void main(void) 
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord) * IN.colour;
	vec3 incident = normalize(IN.worldPos - cameraPos);
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2, 1.0);
	vec3 unitNormal = normalize(IN.normal);
	vec4 reflection = texture(cubeTex, reflect(incident, unitNormal));

	FragColour = (lightColour * diffuse * atten) * (diffuse + reflection);
	FragColour.rgb = reflection.rgb; // Since the model has no texture atm
}

