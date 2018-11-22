#version 440 core

uniform sampler2D diffuseTex;
uniform vec3 lightPos;
uniform float farPlane;

in Vertex {
	vec2 texCoord;
	vec4 FragPos;
} IN;

void main()
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
		
	if (diffuse.a == 0)
		discard;
	else 
	{
		float lightDistance = length(FragPos.xyz - lightPos);
		lightDistance = lightDistance / farPlane;
		gl_FragDepth = lightDistance;
	}
} 