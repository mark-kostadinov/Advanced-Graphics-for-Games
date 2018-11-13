#version 440 core

uniform sampler2D diffuseTex;
uniform vec3 lightPos;
uniform vec3 cameraPos;

//0 linear; 1 exponential; 2 exponential square
//uniform int fogSelector;
//0 plane based; 1 range based
//uniform int depthFog;

in Vertex {
  	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec4 viewSpace;
} IN;

out vec4 FragColour;

int fogSelector = 2;
int depthFog = 0;
 
const vec3 DiffuseLight = vec3(0.15, 0.05, 0.0);
const vec3 RimColor = vec3(0.2, 0.2, 0.2);
const vec3 fogColor = vec3(0.5, 0.5, 0.5);
const float FogDensity = 0.05;
 
void main()
{
	vec3 tex1 = texture(diffuseTex, IN.texCoord).rgb;
	 
	//get light an view directions
	vec3 L = normalize(lightPos - IN.position);
	vec3 V = normalize(cameraPos - IN.position);
	 
	//diffuse lighting
	vec3 diffuse = DiffuseLight * max(0, dot(L, IN.normal));
	 
	//rim lighting
	float rim = 1 - max(dot(V, IN.normal), 0.0);
	rim = smoothstep(0.6, 1.0, rim);
	vec3 finalRim = RimColor * vec3(rim, rim, rim);
	//get all lights and texture
	vec3 lightColor = finalRim + diffuse + tex1;
	vec3 finalColor = vec3(0, 0, 0);
	 
	//distance
	float dist = 0;
	float fogFactor = 0;
	 
	//compute distance used in fog equations
	if(depthFog == 0)//select plane based vs range based
	{
	  //plane based
	  dist = abs(IN.viewSpace.z);
	  //dist = (gl_FragCoord.z / gl_FragCoord.w);
	}
	else
	{
	   //range based
	   dist = length(IN.viewSpace);
	}
	 
	if(fogSelector == 0)//linear fog
	{
	   // 20 - fog starts; 80 - fog ends
	   fogFactor = (80 - dist)/(80 - 20);
	   fogFactor = clamp( fogFactor, 0.0, 1.0 );
	 
	   //if you inverse color in glsl mix function you have to
	   //put 1.0 - fogFactor
	   finalColor = mix(fogColor, lightColor, fogFactor);
	}
	else if( fogSelector == 1)// exponential fog
	{
		fogFactor = 1.0 /exp(dist * FogDensity);
		fogFactor = clamp( fogFactor, 0.0, 1.0 );
	 
		// mix function fogColor⋅(1−fogFactor) + lightColor⋅fogFactor
		finalColor = mix(fogColor, lightColor, fogFactor);
	}
	else if( fogSelector == 2)
	{
	   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
	   fogFactor = clamp( fogFactor, 0.0, 1.0 );
	 
	   finalColor = mix(fogColor, lightColor, fogFactor);
	}
	 
	//show fogFactor depth(gray levels)
	//fogFactor = 1 - fogFactor;
	//FragColour = vec4( fogFactor, fogFactor, fogFactor,1.0 );
	FragColour = vec4(finalColor, 1);
}