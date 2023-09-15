#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int explosion;
uniform float time;
uniform float start_time;
uniform mat4 M;    
uniform mat4 V;    
uniform mat4 P;  
uniform mat4 S; 

in VS_OUT {
	vec2 texCoord;
	vec3 normal;
	vec4 worldPos;
	vec3 ViewDirection;
	vec3 Normal;
	vec3 Position;
} gs_in[];

out vec3 normal;
out vec2 texCoord;
out vec4 worldPos; 
out vec3 ViewDirection;
out vec3 Normal;
out vec3 Position;

float speed = 5;
float accelerationValue = 5;

void main()
{	

		vec3 v1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
		vec3 v2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
		vec3 n = normalize(cross(v1, v2));

		float real_time = time - start_time;
		float s = speed * real_time + 0.5 * accelerationValue * pow(real_time, 2);

		for(int i = 0; i < 3; i++){
			
			gl_Position = P *  V * M * S * vec4(gl_in[i].gl_Position.xyz + n * s, gl_in[i].gl_Position.w);
			texCoord = gs_in[i].texCoord;
			
			worldPos = M * vec4(gs_in[i].worldPos.xyz + n * s, gl_in[i].gl_Position.w);
			normal = gs_in[i].normal;
			ViewDirection = gs_in[i].ViewDirection;
			Normal = gs_in[i].Normal;
			Position = gs_in[i].Position;
			
			EmitVertex();
		}
		EndPrimitive();

}