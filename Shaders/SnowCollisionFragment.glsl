#version 330 core

uniform float particlePosX;
uniform float particlePosZ;
uniform float terrainSize;
uniform float terrainGridSize;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	vec2 particleCoords = vec2(particlePosX, particlePosZ);
	// Default normalize won't do the trick
	particleCoords = particleCoords / (terrainSize * terrainGridSize);

	float dist = distance(particleCoords, IN.texCoord);
	
	// If a snowflake has hit the ground - mark the spot
	if(dist < 0.002f) { /// 0.002f for normal size snowflakes
		FragColor = vec4(0.1, 0.1, 0.1, 1.0);
		//FragColor = mix(vec4(0.1, 0.1, 0.1, 1.0), vec4(texCoord.xy, 0.0, 0.0), 1.0);
	} 
	// Else just leave it blank (i.e. black)
	else {
		FragColor = vec4 (0, 0, 0, 1);
	}
}