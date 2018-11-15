#version 440 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform int hasBumpTexture;
uniform sampler2DShadow shadowTex;
uniform sampler2D snowBufferColourTexture;
uniform sampler2D rainBufferColourTexture;
uniform bool renderSnow;
uniform bool renderRain;
uniform float terrainSize;
uniform float terrainGridSize;

uniform vec3	cameraPos;
uniform vec4	lightColour;
uniform vec3	lightPos;
uniform float	lightRadius;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 FragColour;

void main(void)	
{
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
		
	if (diffuse.a == 0)
		discard;
	else 
	{
		mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
		vec3 normal = normalize(TBN * (texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
		if (hasBumpTexture == 0) {
			normal = IN.normal;
		}
		
		vec3 incident = normalize(lightPos - IN.worldPos);
		float lambert = max(0.0, dot(incident, normal));
		
		float dist = length(lightPos - IN.worldPos);
		float atten	= 1.0 - clamp(dist / lightRadius, 0.0, 1.0);
		
		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);
		
		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, 33.0);
		
		float shadow = 1.0; 
		
		if (IN.shadowProj.w > 0.0) {
			shadow = textureProj(shadowTex, IN.shadowProj);
		}
		
		lambert *= shadow;	// looks cooler if it's stronger
		vec3 colour	= diffuse.rgb * lightColour.rgb;
		colour += (lightColour.rgb * sFactor) * 0.33;
		float ambience = 0.85;
		
		FragColour = vec4(colour * atten * lambert, diffuse.a);
		FragColour.rgb += (diffuse.rgb * lightColour.rgb) * ambience;
		
		if (renderSnow) 
		{
			// HeightMap side size = 512.0f; Each grid size is 32.0f => So, to normalize the coords, divide them by (512 / 32 = 16)
			vec4 snowAccum = texture(snowBufferColourTexture, IN.texCoord.xy / (terrainSize / terrainGridSize));
			FragColour.rgb += snowAccum.rgb;
		}
		if (renderRain) 
		{
			vec4 rainGloss = texture(rainBufferColourTexture, IN.texCoord.xy / (terrainSize / terrainGridSize));
			FragColour.b += rainGloss.b * 0.1;
		}
		
		// Debug shadows
		float t = textureProj(shadowTex, IN.shadowProj);
		FragColour.rgb = vec3(t, t, t);
	}
}