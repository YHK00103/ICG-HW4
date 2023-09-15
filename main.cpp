#include <iostream>
#include <math.h>
#include <vector>
#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object.h"

#define 	GLFW_KEY_1 49
#define 	GLFW_KEY_2  50
#define 	GLFW_KEY_3  51
#define 	GLFW_KEY_4  52
#define 	GLFW_KEY_5  53
#define 	GLFW_KEY_6  54
#define 	GLFW_KEY_7  55
#define 	GLFW_KEY_8  56
#define 	GLFW_KEY_9  57

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int createShader(const char* filename, const char* type);
unsigned int createProgram(unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader);
void LoadTexture(unsigned int& texture, const char* tFileName);
unsigned int LoadCubeMap(vector<string> texture_faces);
void DrawModel(const char* target, glm::mat4 M, glm::mat4 V, glm::mat4 P, glm::mat4 S, unsigned int Program);
unsigned int ModelVAO(Object* model);
unsigned int SkyboxModelVAO(Object* model);
glm::mat4 getPerspective();
glm::mat4 getView();

Object* catModel = new Object("obj/cat.obj");
Object* boxModel = new Object("obj/CardboardBox1.obj");
Object* cubeModel = new Object("obj/cube.obj");

unsigned int catVAO, boxVAO, cubeVAO;
unsigned int vertexShader, fragmentShader, geometryShader, shaderProgram, normalProgram, explosionProgram, discardProgram;
unsigned int skyBoxVertexShader, skyBoxFragmentShader, skyBoxProgram;

int windowWidth = 800, windowHeight = 600;

double cat_size = 1.0;
double add = 1.0;
bool press[10];
bool keys[10];
int changeColor = 0;
int deformation = 0;
int explosion = 0;
int discard = 0;
float timer, start_time = 0.0;
vector<string> texture_faces;

float angle = 0;
float lighting = 1;
float squeezeFactor = 0;
bool squeezing = false;
bool lightChanging = false;

float skyboxVertices[] = {
	//back
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	// left
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	//right
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	//front
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	//top
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	//bottom
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
};

int main()
{
	// Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFE_OPENGL_FORWARD_COMPACT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "HW4", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Shaders
	vertexShader = createShader("shaders/vertexShader.vert", "vert");
	fragmentShader = createShader("shaders/fragmentShader.frag", "frag");
	shaderProgram = createProgram(vertexShader, 0, fragmentShader);

	vertexShader = createShader("shaders/cat.vert", "vert");
	geometryShader = createShader("shaders/cat.geom", "geom");
	fragmentShader = createShader("shaders/cat.frag", "frag");
	explosionProgram = createProgram(vertexShader, geometryShader, fragmentShader);

	vertexShader = createShader("shaders/normal.vert", "vert");
	geometryShader = createShader("shaders/normal.geom", "geom");
	fragmentShader = createShader("shaders/normal.frag", "frag");
	normalProgram = createProgram(vertexShader, geometryShader, fragmentShader);

	skyBoxVertexShader = createShader("shaders/skybox.vert", "vert");
	skyBoxFragmentShader = createShader("shaders/skybox.frag", "frag");
	skyBoxProgram = createProgram(skyBoxVertexShader, 0, skyBoxFragmentShader);

	// Texture
	unsigned int catTexture, boxTexture, discardTexture, fireTexture;
	LoadTexture(catTexture, "obj/Cat_diffuse.jpg");
	LoadTexture(discardTexture, "obj/discard_texture.jpg");
	//LoadTexture(boxTexture, "obj/CardboardBox1_Albedo.tga");

	//CubeMap
	unsigned int cubeMap;
	texture_faces.push_back("obj/right.jpg");
	texture_faces.push_back("obj/left.jpg");
	texture_faces.push_back("obj/bottom.jpg");
	texture_faces.push_back("obj/top.jpg");
	texture_faces.push_back("obj/back.jpg");
	texture_faces.push_back("obj/front.jpg");
	cubeMap = LoadCubeMap(texture_faces);

	// TO DO:
	// Create VAO, VBO
	catVAO = ModelVAO(catModel);
	//boxVAO = ModelVAO(boxModel);
	cubeVAO = SkyboxModelVAO(cubeModel);

	// Display loop
	// TO DO:
	// Draw the models with shaders
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Initialize press & keys
	for (int i = 0; i < 10; i++) {
		press[i] = false;
		keys[i] = false;
	}

	// Display loop
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	double dt;
	double lastTime = glfwGetTime();
	double currentTime;
	unsigned int mLoc, vLoc, pLoc, normalMLoc;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSetKeyCallback(window, keyCallback);

		//skybox program
		if ((changeColor == 3) || (changeColor == 4)) {

			glUseProgram(skyBoxProgram);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			//glm::mat4 Rotate = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f) * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
			//DrawModel("cube", Rotate, getView(), getPerspective(), glm::mat4(1.0f), skyBoxProgram);
			DrawModel("cube", glm::mat4(1.0f), glm::mat4(1.0f), getPerspective(), glm::mat4(1.0f), skyBoxProgram);
			glUseProgram(0);
		}

		//shader program
		glUseProgram(shaderProgram);
		glActiveTexture(GL_TEXTURE0);
		glm::mat4 box_model = glm::mat4(1.0f);
		glm::mat4 cat_model = glm::mat4(1.0f);
		glm::mat4 cat_scale_model = glm::mat4(1.0f);

		//Change color
		timer = glfwGetTime();
		glUniform1i(glGetUniformLocation(shaderProgram, "changeColor"), changeColor);
		glUniform1f(glGetUniformLocation(shaderProgram, "time"), timer);
		glUniform1i(glGetUniformLocation(shaderProgram, "deformation"), deformation);
		glUniform1i(glGetUniformLocation(shaderProgram, "cubeMap"), 1);

		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

		//discard
		if (changeColor == 6) {
			glBindTexture(GL_TEXTURE_2D, discardTexture);

			float fFactor = fmod(glfwGetTime(), 10.0f);
			fFactor /= 10.0f;
			glUniform1f(glGetUniformLocation(shaderProgram, "dissolveFactor"), fFactor);
		}

		//Cat
		glBindTexture(GL_TEXTURE_2D, catTexture);

		//Transformation
		if (explosion != 1) {
			cat_model = glm::rotate(cat_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			cat_model = glm::rotate(cat_model, glm::radians(45.0f) * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
			DrawModel("cat", cat_model, getView(), getPerspective(), cat_scale_model, shaderProgram);
		}
		

		//Geometry shader
		if (press[7] == true && explosion == 0) {
			glUseProgram(normalProgram);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, catTexture);

			glUniform1i(glGetUniformLocation(normalProgram, "changeColor"), changeColor);
			glUniform1f(glGetUniformLocation(normalProgram, "time"), timer);
			//glUniform1i(glGetUniformLocation(normalProgram, "deformation"), deformation);
			glUniform1i(glGetUniformLocation(normalProgram, "cubeMap"), 1);

			//normalM
			//glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 normalM = glm::transpose(glm::inverse(cat_model));
			normalMLoc = glGetUniformLocation(normalProgram, "normalM");
			glUniformMatrix4fv(normalMLoc, 1, GL_FALSE, glm::value_ptr(normalM));
			DrawModel("cat", cat_model, getView(), getPerspective(), glm::mat4(1.0f), normalProgram);

		}

		if (explosion == 1) {
			glUseProgram(explosionProgram);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, catTexture);

			glUniform1f(glGetUniformLocation(explosionProgram, "start_time"), start_time);
			glUniform1f(glGetUniformLocation(explosionProgram, "time"), timer);
			glUniform1i(glGetUniformLocation(explosionProgram, "explosion"), explosion);

			DrawModel("cat", cat_model, getView(), getPerspective(), glm::mat4(1.0f), explosionProgram);
		}

		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void DrawModel(const char* target, glm::mat4 M, glm::mat4 V, glm::mat4 P, glm::mat4 S, unsigned int Program) {

	glUniformMatrix4fv(glGetUniformLocation(Program, "P"), 1, GL_FALSE, glm::value_ptr(P));
	glUniformMatrix4fv(glGetUniformLocation(Program, "V"), 1, GL_FALSE, glm::value_ptr(V));
	glUniformMatrix4fv(glGetUniformLocation(Program, "M"), 1, GL_FALSE, glm::value_ptr(M));
	glUniformMatrix4fv(glGetUniformLocation(Program, "S"), 1, GL_FALSE, glm::value_ptr(S));

	if (strcmp(target, "box") == 0)
	{
		glBindVertexArray(boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, boxModel->positions.size());
	}
	else if (strcmp(target, "cat") == 0)
	{
		glBindVertexArray(catVAO);
		glDrawArrays(GL_TRIANGLES, 0, catModel->positions.size());
	}
	else if (strcmp(target, "cube") == 0)
	{
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (action == GLFW_PRESS) {
		keys[key - 49] == true;

		if (key == GLFW_KEY_1) {
			cout << "press 1" << endl;
			press[1] = true;
			press[2] = false;
			press[3] = false;
			press[4] = false;
			press[5] = false;
			press[6] = false;
			press[7] = false;
			press[8] = false;
			changeColor = 0;
			explosion = 0;
		}
		else if (key == GLFW_KEY_2) {
			cout << "press 2" << endl;
			press[2] = true;
			changeColor = 1;
			explosion = 0;
		}
		else if (key == GLFW_KEY_3) {
			cout << "press 3" << endl;
			changeColor = 2;
			explosion = 0;
		}
		else if (key == GLFW_KEY_4) {
			cout << "press 4" << endl;
			changeColor = 3;
			explosion = 0;
		}
		else if (key == GLFW_KEY_5) {
			cout << "press 5" << endl;
			changeColor = 4;
			explosion = 0;
		}
		else if (key == GLFW_KEY_6) {
			cout << "press 6" << endl;
			explosion = 1;
			start_time = timer;
			press[6] = true;
		}
		else if (key == GLFW_KEY_7) {
			cout << "press 7" << endl;
			press[7] = true;
			explosion = 0;
		}
		else if (key == GLFW_KEY_8) {
			cout << "press 8" << endl;
			changeColor = 6;
			press[8] = true;

		}
		else if (key == GLFW_KEY_9) {
			cout << "press 9" << endl;
			press[9] = true;
		}

	}
	else if (action == GLFW_RELEASE) {
		keys[key - 49] == false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

unsigned int createShader(const char* filename, const char* type)
{
	unsigned int shader;
	if (strcmp(type, "vert") == 0)
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (strcmp(type, "frag") == 0)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else if (strcmp(type, "geom") == 0)
		shader = glCreateShader(GL_GEOMETRY_SHADER);
	else
	{
		cout << "Unknown Shader Type.\n";
		return 0;
	}

	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  //same as rewind(fp);

	char* source = (char*)malloc(sizeof(char) * (fsize + 1));
	fread(source, fsize, 1, fp);
	fclose(fp);

	source[fsize] = 0;

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::" << type << "::COMPLIATION_FAILED\n" << infoLog << endl;
		return -1;
	}

	return shader;
}

unsigned int createProgram(unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader)
{
	unsigned int program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, geometryShader);
	glAttachShader(program, fragmentShader);

	//Link our program
	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		int maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char* infoLog = (char*)malloc(sizeof(char) * (maxLength));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		cout << "LINKING ERROR: ";
		puts(infoLog);
		free(infoLog);

		return -1;
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return program;
}

unsigned int LoadCubeMap(vector<string> texture_faces) {

	unsigned int textureID;
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < texture_faces.size(); i++)
	{
		unsigned char* data = stbi_load(texture_faces[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			/*
			if (i == 1)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);  //right
			else if (i == 2)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);  //left
			else if (i == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);   //bottom
			else if (i == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);   //top
			else if (i == 5)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);   //back
			else if (i == 6)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);   //front
			*/
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << texture_faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void LoadTexture(unsigned int& texture, const char* tFileName) {

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	if (texture == (size_t)"catTexture")
		glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 1);
	else if (texture == (size_t)"boxture")
		glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

glm::mat4 getPerspective()
{
	return glm::perspective(
		glm::radians(45.0f),
		(float)windowWidth / (float)windowHeight,
		0.1f, 100.0f);
}

glm::mat4 getView()
{
	return glm::lookAt(
		glm::vec3(0, 5, 5),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
}

unsigned int ModelVAO(Object* model)
{
	unsigned int VAO, VBO[3];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->positions.size()), &(model->positions[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->normals.size()), &(model->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->texcoords.size()), &(model->texcoords[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

unsigned int SkyboxModelVAO(Object* model) {

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

