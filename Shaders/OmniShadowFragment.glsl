#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 lightPos;
uniform float farPlane;

in vec2 texCoord;
in vec4 FragPos;

void main()
{
	vec4 diffuse = texture(diffuseTex, texCoord);
	vec3 fragPosition = FragPos.xyz;
		
	if (diffuse.a == 0)
		discard;
	else 
	{
		float lightDistance = length(fragPosition - lightPos);
		lightDistance = lightDistance / farPlane;
		gl_FragDepth = lightDistance;
	}
} 