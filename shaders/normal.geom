#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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

uniform mat4 P;
uniform int explosion;
uniform float start_time;

const float MAGNITUDE = 0.1;

float speed = 10;
float accelerationValue = 10;

void GenerateLine(int index)
{

	gl_Position = P * gl_in[index].gl_Position;
	texCoord = gs_in[index].texCoord;
	worldPos = gs_in[index].worldPos;
	normal = gs_in[index].normal;
	ViewDirection = gs_in[index].ViewDirection;
	Normal = gs_in[index].Normal;
	Position = gs_in[index].Position;
	EmitVertex();

	gl_Position = P * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
	texCoord = gs_in[index].texCoord;
	worldPos = gs_in[index].worldPos;
	normal = gs_in[index].normal;
	ViewDirection = gs_in[index].ViewDirection;
	Normal = gs_in[index].Normal;
	Position = gs_in[index].Position;
	EmitVertex();
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
	EndPrimitive();
}