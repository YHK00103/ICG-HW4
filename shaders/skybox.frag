#version 330 core

// TO DO:
// Implement fragment shader

in vec3 texCoord;

uniform samplerCube cubeMap;
uniform float time;

out vec4 FragColor;

void main(){
	
		FragColor = texture(cubeMap, texCoord);
}