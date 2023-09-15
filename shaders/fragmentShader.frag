#version 330 core

// TO DO:
// Implement fragment shader

in vec4 worldPos;
in vec3 normal;
in vec2 texCoord;
in vec3 ViewDirection;
in vec3 Normal;
in vec3 Position;

uniform sampler2D Texture;
uniform sampler2D discardTexture;
uniform samplerCube cubeMap;
uniform int changeColor;
uniform float time;
uniform float dissolveFactor;
uniform mat4 Rotate;

out vec4 FragColor;

const float pearl_brightness = 1.5;

bool inRange(float color1, float color2){
	return color1 - color2 < 0.01;
}

void main(){

	if(changeColor == 0){
		FragColor = texture(Texture, texCoord);	
	}		
	else if(changeColor == 1){
		FragColor = texture(Texture, texCoord);
		if(inRange(FragColor.r, 118.0/255.0) || inRange(FragColor.g, 52.0/255.0) || inRange(FragColor.b, 23.0/255.0))
			FragColor = vec4(54.0/255.0, 23.0/255.0, 11.0/255.0, 1);
	}
	else if(changeColor == 2){
		vec2 uv = texCoord;
		vec3 lightPos = vec3(20, 10, 10);
		vec3 n_normal = normalize(normal);
		vec3 light = (normalize(vec4(lightPos, 1.0) - worldPos)).xyz;

		float diffuse = max(dot(light, n_normal), 0.0);

		float red = 0.5 + 0.5 * sin(time * 1.0 + uv.x * 3.0 + uv.y * 0.0);
		float green = 0.5 + 0.5 * sin(time * 0.5 + uv.x * 0.0 + uv.y * 3.0);
		float blue = 0.5 + 0.5 * sin(time * 0.75 + uv.x * 2.0 + uv.y * 2.0);

		FragColor = vec4(red, green, blue, 1.0) + vec4(diffuse, diffuse, diffuse, 1);
	}
	else if(changeColor == 3){

		vec3 I = normalize(Position - (0, 5, 5));
		vec3 R =  -reflect(I, normalize(Normal));
		FragColor =  texture(cubeMap, R);
	}
	else if(changeColor == 4){
	
		const float peal_brightness = 1.5;

		vec3 I = normalize(Position - (0, 5, 5));
		vec3 R = -reflect(I, normalize(Normal));
		vec3 fvNormal = normalize(Normal);

		float view_dot_normal = max(dot(fvNormal, I), 0.0);
		float view_dot_normal_inverse = 1.0 - view_dot_normal;

		float red = texture(cubeMap, R).r * view_dot_normal + pearl_brightness  * texture(cubeMap, R + vec3(0.1, 0.0, 0.0) * view_dot_normal_inverse).r * (1.0 - view_dot_normal);
		float green = texture(cubeMap, R).g * view_dot_normal + pearl_brightness  * texture(cubeMap, R + vec3(0.0, 0.1, 0.0) * view_dot_normal_inverse).g * (1.0 - view_dot_normal);
		float blue = texture(cubeMap, R).b * view_dot_normal + pearl_brightness  * texture(cubeMap, R + vec3(0.0, 0.0, 0.1) * view_dot_normal_inverse).b * (1.0 - view_dot_normal);
		FragColor = vec4(red, green, blue, 1.0);
	}
	else if(changeColor == 5){
	
		 float ratio = 1.00 / 1.52;     
		 vec3 I = normalize(Position - (0, 5, 5));
		 vec3 R = refract(I, normalize(Normal), ratio);
		 FragColor = vec4(texture(cubeMap, R).rgb, 1.0);
	}
	else if(changeColor == 6){

		vec4 discardFrag = texture(discardTexture, texCoord);

		FragColor = texture(Texture, texCoord);	

		if(discardFrag.r < dissolveFactor)
			discard;

		//FragColor = texture(Texture, texCoord);	
		
	}
}