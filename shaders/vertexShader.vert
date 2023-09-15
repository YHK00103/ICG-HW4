#version 330 core

// TO DO:
// Implement vertex shader
// note: remember to set gl_Position

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 M;    
uniform mat4 V;    
uniform mat4 P;  
uniform mat4 S; 
uniform float time;
uniform int deformation;

out vec4 worldPos;
out vec3 normal;
out vec2 texCoord;
out vec3 Normal;
out vec3 Position;

void main(){
	/*
	if(deformation== 0){
		
	}
	else if(deformation == 1){
		vec3 object_pos;
		object_pos.x = aPosition.x + 1.0 * time;
		object_pos.y = aPosition.y + 1.0 * time + 9.8/2.0 * time * time;
		object_pos.z = aPosition.z + 1.0 * time;
		gl_Position = P * V * M * S * vec4(object_pos, 1.0f);
		
		vec3 object_pos = aPosition;
		float s = 1.0 + 0.1 * sin(aPosition.x * time) * sin(aPosition.z * time);
		object_pos.y = s * aPosition.y;
		gl_Position = P * V * M * S * vec4(object_pos, 1.0f);
		
	}
	*/
	
	gl_Position = P * V * M * S * vec4(aPosition, 1.0f);
	texCoord = aTexCoord;

	//lighting
	worldPos = M * S * vec4(aPosition, 1.0f);
	mat4 normal_transform = transpose(inverse(M*S));
	normal = normalize((normal_transform * vec4(aNormal, 0.0)).xyz);

	//Environmentl mapping
	Normal = mat3(transpose(inverse(M*S))) * aNormal;
	Position = vec3( M * S * vec4(aPosition, 1.0f));
}