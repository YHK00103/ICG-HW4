#version 330 core

// TO DO:
// Implement vertex shader
// note: remember to set gl_Position

layout (location = 0) in vec3 aPosition;
  
uniform mat4 V;    
uniform mat4 P;  
uniform mat4 M; 

out vec3 texCoord;

void main(){
	
	texCoord = vec3(aPosition.x, -aPosition.yz);
	vec4 pos =  P * V * M * vec4(aPosition, 1.0f);
	gl_Position = pos.xyww;

	/*
	texCoord = aPosition;
	gl_Position = V * vec4(aPosition, 1.0f);
	float w = -gl_Position.z;
	gl_Position = P * V * vec4(aPosition, 1.0f);
	gl_Position.z = w;
	*/
}