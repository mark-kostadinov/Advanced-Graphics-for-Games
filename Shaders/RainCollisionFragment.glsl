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
	
	//FragColor.xy = particleCoords;
	//FragColor.xy = IN.texCoord;

	float dist = distance(particleCoords, IN.texCoord);
	
	// If a raindrop has hit the ground - mark the spot
	if(dist < 0.014f) { /// 0.001f for normal size raindrops
		FragColor = vec4(0.98, 0.98, 0.98, 0.5);
	} 
	// Else just leave it blank (i.e. black)
	else {
		FragColor = vec4 (0, 0, 0, 1);
	}
}