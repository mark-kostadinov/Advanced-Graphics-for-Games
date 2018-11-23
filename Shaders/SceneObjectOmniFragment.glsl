#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform int hasBumpTexture;
uniform bool shadowDebugMode;
uniform samplerCube omniShadowMap;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;
uniform float farPlane;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 FragColour;

// Credit for the function: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos; 
    float closestDepth = texture(omniShadowMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = 0.05; 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	vec4 texColour = texture(diffuseTex, IN.texCoord);
		
	if (texColour.a == 0)
		discard;
	else 
	{
		mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
		vec3 normal = normalize(TBN * (texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
		if (hasBumpTexture == 0) {
			normal = normalize(IN.normal);
		}
		vec3 colour = texColour.rgb;
		vec3 ambient = 0.3 * colour;
		
		vec3 incident = normalize(lightPos - IN.worldPos);
		float lambert = max(0.0, dot(incident, normal));
		
		float dist = length(lightPos - IN.worldPos);
		float atten	= 1.0 - clamp(dist / lightRadius, 0.0, 1.0);
		
		vec3 diffuse = lambert * lightColour.rgb;
		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);
 
		float rFactor = max(0.0, dot(halfDir, normal));
		float sFactor = pow(rFactor, 33.0);
		vec3 specular = sFactor * lightColour.rgb;    

		float shadow = ShadowCalculation(IN.worldPos);                      
		vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * colour;    
		
		FragColour = vec4(lighting, 1.0);
		
		// Debug shadows
		if (shadowDebugMode) {
			float t = texture(omniShadowMap, IN.worldPos - lightPos).r;
			FragColour.rgb = vec3(t, t, t);
		}
	}
}