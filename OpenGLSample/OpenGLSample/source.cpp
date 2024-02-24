﻿
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShapeGenerator.h"
#include "ShapeData.h"



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"
#include "camera.h"

#include <iostream>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
const int STRIDE = 7;

GLuint sphereNumIndices;
GLuint sphereVertexArrayObjectID;
GLuint sphereIndexByteOffset;

GLuint sphereNumIndices2;
GLuint sphereVertexArrayObjectID2;
GLuint sphereIndexByteOffset2;

// projection matrix
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
bool perspective = true;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float verticesCube[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// positions all containers
	glm::vec3 cubePositions[] = {
		//Left Side Stairs
		glm::vec3(-5.0f, 5.0f,  0.0f),
		glm::vec3(-4.0f, 4.0f, 0.0f),
		glm::vec3(-3.0f, 3.0f, 0.0f),
		glm::vec3(-2.0f, 2.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),

		//Center Stairs
		glm::vec3(-5.0f, 5.0f,  1.0f),
		glm::vec3(-4.0f, 4.0f, 1.0f),
		glm::vec3(-3.0f, 3.0f, 1.0f),
		glm::vec3(-2.0f, 2.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),

		//Right Side Stairs
		glm::vec3(-5.0f, 5.0f,  2.0f),
		glm::vec3(-4.0f, 4.0f, 2.0f),
		glm::vec3(-3.0f, 3.0f, 2.0f),
		glm::vec3(-2.0f, 2.0f, 2.0f),
		glm::vec3(-1.0f, 1.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 2.0f)
	};
	auto numCubes = std::size(cubePositions);

	glm::vec3 bannCubePositions[] = {
		glm::vec3(1.0f, 0.0f, 8.0f),
		glm::vec3(1.0f, 1.0f, 8.0f),
		glm::vec3(1.0f, 2.0f, 8.0f),
		glm::vec3(1.0f, 3.0f, 8.0f),
		glm::vec3(1.0f, 4.0f, 8.0f),
		glm::vec3(1.0f, 5.0f, 8.0f),
		glm::vec3(1.0f, 6.0f, 8.0f),
		glm::vec3(1.0f, 7.0f, 8.0f),

	};
	auto numSmallCubes = std::size(bannCubePositions);

	glm::vec3 railCubePositions[] = {
		glm::vec3(7.0f, 6.0f, 12.0f),
		glm::vec3(7.0f, 7.0f, 12.0f),
		glm::vec3(7.0f, 8.0f, 12.0f),
		glm::vec3(7.0f, 9.0f, 12.0f),
		glm::vec3(7.0f, 10.0f, 12.0f),
		glm::vec3(7.0f, 11.0f, 12.0f),
		glm::vec3(7.0f, 12.0f, 12.0f),
		glm::vec3(7.0f, 13.0f, 12.0f),
		glm::vec3(7.0f, 14.0f, 12.0f),
		glm::vec3(7.0f, 15.0f, 12.0f),
		glm::vec3(7.0f, 16.0f, 12.0f),
		glm::vec3(7.0f, 17.0f, 12.0f),
		glm::vec3(7.0f, 18.0f, 12.0f),
		glm::vec3(7.0f, 19.0f, 12.0f),
		glm::vec3(7.0f, 20.0f, 12.0f),
		glm::vec3(7.0f, 21.0f, 12.0f),
		glm::vec3(7.0f, 22.0f, 12.0f),
		glm::vec3(7.0f, 23.0f, 12.0f),
		glm::vec3(7.0f, 24.0f, 12.0f),
		glm::vec3(7.0f, 25.0f, 12.0f),
		glm::vec3(7.0f, 26.0f, 12.0f),
		glm::vec3(7.0f, 27.0f, 12.0f),
		glm::vec3(7.0f, 28.0f, 12.0f),
		glm::vec3(7.0f, 29.0f, 12.0f),
		glm::vec3(7.0f, 30.0f, 12.0f),
		glm::vec3(7.0f, 31.0f, 12.0f),
		glm::vec3(7.0f, 32.0f, 12.0f),
		glm::vec3(7.0f, 33.0f, 12.0f),
		glm::vec3(7.0f, 34.0f, 12.0f),
		glm::vec3(7.0f, 35.0f, 12.0f),
		glm::vec3(7.0f, 36.0f, 12.0f),
		glm::vec3(7.0f, 37.0f, 12.0f),
		glm::vec3(7.0f, 38.0f, 12.0f),
		glm::vec3(7.0f, 39.0f, 12.0f),
		glm::vec3(7.0f, 40.0f, 12.0f),
		glm::vec3(7.0f, 41.0f, 12.0f),

	};
	auto numRailCubes = std::size(railCubePositions);

	glm::vec3 vertRailPositions[] = {
		//First Post
		glm::vec3(-2.0f, 5.0f, 24.0f),
		glm::vec3(-2.0f, 6.0f, 24.0f),
		glm::vec3(-2.0f, 7.0f, 24.0f),
		glm::vec3(-2.0f, 8.0f, 24.0f),
		glm::vec3(-2.0f, 9.0f, 24.0f),
		glm::vec3(-2.0f, 10.0f, 24.0f),
		glm::vec3(-2.0f, 11.0f, 24.0f),
		glm::vec3(-2.0f, 12.0f, 24.0f),
		glm::vec3(-2.0f, 13.0f, 24.0f),
		glm::vec3(-2.0f, 14.0f, 24.0f),
		glm::vec3(-2.0f, 15.0f, 24.0f),
		glm::vec3(-2.0f, 16.0f, 24.0f),
		glm::vec3(-2.0f, 17.0f, 24.0f),
		glm::vec3(-2.0f, 18.0f, 24.0f),
		glm::vec3(-2.0f, 19.0f, 24.0f),
		glm::vec3(-2.0f, 20.0f, 24.0f),
		glm::vec3(-2.0f, 21.0f, 24.0f),
		glm::vec3(-2.0f, 22.0f, 24.0f),

		//Second Post
		glm::vec3(-7.0f, 9.0f, 24.0f),
		glm::vec3(-7.0f, 10.0f, 24.0f),
		glm::vec3(-7.0f, 11.0f, 24.0f),
		glm::vec3(-7.0f, 12.0f, 24.0f),
		glm::vec3(-7.0f, 13.0f, 24.0f),
		glm::vec3(-7.0f, 14.0f, 24.0f),
		glm::vec3(-7.0f, 15.0f, 24.0f),
		glm::vec3(-7.0f, 16.0f, 24.0f),
		glm::vec3(-7.0f, 17.0f, 24.0f),
		glm::vec3(-7.0f, 18.0f, 24.0f),
		glm::vec3(-7.0f, 19.0f, 24.0f),
		glm::vec3(-7.0f, 20.0f, 24.0f),
		glm::vec3(-7.0f, 21.0f, 24.0f),
		glm::vec3(-7.0f, 22.0f, 24.0f),
		glm::vec3(-7.0f, 23.0f, 24.0f),
		glm::vec3(-7.0f, 24.0f, 24.0f),
		glm::vec3(-7.0f, 25.0f, 24.0f),
		glm::vec3(-7.0f, 26.0f, 24.0f),

		//Third Post
		glm::vec3(-12.0f, 13.0f, 24.0f),
		glm::vec3(-12.0f, 14.0f, 24.0f),
		glm::vec3(-12.0f, 15.0f, 24.0f),
		glm::vec3(-12.0f, 16.0f, 24.0f),
		glm::vec3(-12.0f, 17.0f, 24.0f),
		glm::vec3(-12.0f, 18.0f, 24.0f),
		glm::vec3(-12.0f, 19.0f, 24.0f),
		glm::vec3(-12.0f, 20.0f, 24.0f),
		glm::vec3(-12.0f, 21.0f, 24.0f),
		glm::vec3(-12.0f, 22.0f, 24.0f),
		glm::vec3(-12.0f, 23.0f, 24.0f),
		glm::vec3(-12.0f, 24.0f, 24.0f),
		glm::vec3(-12.0f, 25.0f, 24.0f),
		glm::vec3(-12.0f, 26.0f, 24.0f),
		glm::vec3(-12.0f, 27.0f, 24.0f),
		glm::vec3(-12.0f, 28.0f, 24.0f),
		glm::vec3(-12.0f, 29.0f, 24.0f),
		glm::vec3(-12.0f, 30.0f, 24.0f),
		glm::vec3(-12.0f, 31.0f, 24.0f),

		//Fourth Post
		glm::vec3(-17.0f, 18.0f, 24.0f),
		glm::vec3(-17.0f, 19.0f, 24.0f),
		glm::vec3(-17.0f, 20.0f, 24.0f),
		glm::vec3(-17.0f, 21.0f, 24.0f),
		glm::vec3(-17.0f, 22.0f, 24.0f),
		glm::vec3(-17.0f, 23.0f, 24.0f),
		glm::vec3(-17.0f, 24.0f, 24.0f),
		glm::vec3(-17.0f, 25.0f, 24.0f),
		glm::vec3(-17.0f, 26.0f, 24.0f),
		glm::vec3(-17.0f, 27.0f, 24.0f),
		glm::vec3(-17.0f, 28.0f, 24.0f),
		glm::vec3(-17.0f, 29.0f, 24.0f),
		glm::vec3(-17.0f, 30.0f, 24.0f),
		glm::vec3(-17.0f, 31.0f, 24.0f),
		glm::vec3(-17.0f, 32.0f, 24.0f),
		glm::vec3(-17.0f, 33.0f, 24.0f),
		glm::vec3(-17.0f, 34.0f, 24.0f),
		glm::vec3(-17.0f, 35.0f, 24.0f),
		glm::vec3(-17.0f, 36.0f, 24.0f),

		//Fifth Post
		glm::vec3(-22.0f, 23.0f, 24.0f),
		glm::vec3(-22.0f, 24.0f, 24.0f),
		glm::vec3(-22.0f, 25.0f, 24.0f),
		glm::vec3(-22.0f, 26.0f, 24.0f),
		glm::vec3(-22.0f, 27.0f, 24.0f),
		glm::vec3(-22.0f, 28.0f, 24.0f),
		glm::vec3(-22.0f, 29.0f, 24.0f),
		glm::vec3(-22.0f, 30.0f, 24.0f),
		glm::vec3(-22.0f, 31.0f, 24.0f),
		glm::vec3(-22.0f, 32.0f, 24.0f),
		glm::vec3(-22.0f, 33.0f, 24.0f),
		glm::vec3(-22.0f, 34.0f, 24.0f),
		glm::vec3(-22.0f, 35.0f, 24.0f),
		glm::vec3(-22.0f, 36.0f, 24.0f),
		glm::vec3(-22.0f, 37.0f, 24.0f),
		glm::vec3(-22.0f, 38.0f, 24.0f),
		glm::vec3(-22.0f, 39.0f, 24.0f),
		glm::vec3(-22.0f, 40.0f, 24.0f),
		glm::vec3(-22.0f, 41.0f, 24.0f),

		//Fifth Post
		glm::vec3(-27.0f, 28.0f, 24.0f),
		glm::vec3(-27.0f, 29.0f, 24.0f),
		glm::vec3(-27.0f, 30.0f, 24.0f),
		glm::vec3(-27.0f, 31.0f, 24.0f),
		glm::vec3(-27.0f, 32.0f, 24.0f),
		glm::vec3(-27.0f, 33.0f, 24.0f),
		glm::vec3(-27.0f, 34.0f, 24.0f),
		glm::vec3(-27.0f, 35.0f, 24.0f),
		glm::vec3(-27.0f, 36.0f, 24.0f),
		glm::vec3(-27.0f, 37.0f, 24.0f),
		glm::vec3(-27.0f, 38.0f, 24.0f),
		glm::vec3(-27.0f, 39.0f, 24.0f),
		glm::vec3(-27.0f, 40.0f, 24.0f),
		glm::vec3(-27.0f, 41.0f, 24.0f),
		glm::vec3(-27.0f, 42.0f, 24.0f),
		glm::vec3(-27.0f, 43.0f, 24.0f),
		glm::vec3(-27.0f, 44.0f, 24.0f),
		glm::vec3(-27.0f, 45.0f, 24.0f),
		glm::vec3(-27.0f, 46.0f, 24.0f),

		// Sixth Post
		glm::vec3(-32.0f, 33.0f, 24.0f),
		glm::vec3(-32.0f, 34.0f, 24.0f),
		glm::vec3(-32.0f, 35.0f, 24.0f),
		glm::vec3(-32.0f, 36.0f, 24.0f),
		glm::vec3(-32.0f, 37.0f, 24.0f),
		glm::vec3(-32.0f, 38.0f, 24.0f),
		glm::vec3(-32.0f, 39.0f, 24.0f),
		glm::vec3(-32.0f, 40.0f, 24.0f),
		glm::vec3(-32.0f, 41.0f, 24.0f),
		glm::vec3(-32.0f, 42.0f, 24.0f),
		glm::vec3(-32.0f, 43.0f, 24.0f),
		glm::vec3(-32.0f, 44.0f, 24.0f),
		glm::vec3(-32.0f, 45.0f, 24.0f),
		glm::vec3(-32.0f, 46.0f, 24.0f),
		glm::vec3(-32.0f, 47.0f, 24.0f),
		glm::vec3(-32.0f, 48.0f, 24.0f),
		glm::vec3(-32.0f, 49.0f, 24.0f),
		glm::vec3(-32.0f, 50.0f, 24.0f),
		glm::vec3(-32.0f, 51.0f, 24.0f),

		// Seventh Post
		glm::vec3(-37.0f, 38.0f, 24.0f),
		glm::vec3(-37.0f, 39.0f, 24.0f),
		glm::vec3(-37.0f, 40.0f, 24.0f),
		glm::vec3(-37.0f, 41.0f, 24.0f),
		glm::vec3(-37.0f, 42.0f, 24.0f),
		glm::vec3(-37.0f, 43.0f, 24.0f),
		glm::vec3(-37.0f, 44.0f, 24.0f),
		glm::vec3(-37.0f, 45.0f, 24.0f),
		glm::vec3(-37.0f, 46.0f, 24.0f),
		glm::vec3(-37.0f, 47.0f, 24.0f),
		glm::vec3(-37.0f, 48.0f, 24.0f),
		glm::vec3(-37.0f, 49.0f, 24.0f),
		glm::vec3(-37.0f, 50.0f, 24.0f),
		glm::vec3(-37.0f, 51.0f, 24.0f),
		glm::vec3(-37.0f, 52.0f, 24.0f),
		glm::vec3(-37.0f, 53.0f, 24.0f),
		glm::vec3(-37.0f, 54.0f, 24.0f),
		glm::vec3(-37.0f, 55.0f, 24.0f),
		glm::vec3(-37.0f, 56.0f, 24.0f),

		// Eighth Post
		glm::vec3(-42.0f, 43.0f, 24.0f),
		glm::vec3(-42.0f, 44.0f, 24.0f),
		glm::vec3(-42.0f, 45.0f, 24.0f),
		glm::vec3(-42.0f, 46.0f, 24.0f),
		glm::vec3(-42.0f, 47.0f, 24.0f),
		glm::vec3(-42.0f, 48.0f, 24.0f),
		glm::vec3(-42.0f, 49.0f, 24.0f),
		glm::vec3(-42.0f, 50.0f, 24.0f),
		glm::vec3(-42.0f, 51.0f, 24.0f),
		glm::vec3(-42.0f, 52.0f, 24.0f),
		glm::vec3(-42.0f, 53.0f, 24.0f),
		glm::vec3(-42.0f, 54.0f, 24.0f),
		glm::vec3(-42.0f, 55.0f, 24.0f),
		glm::vec3(-42.0f, 56.0f, 24.0f),
		glm::vec3(-42.0f, 57.0f, 24.0f),
		glm::vec3(-42.0f, 58.0f, 24.0f),
		glm::vec3(-42.0f, 59.0f, 24.0f),
		glm::vec3(-42.0f, 60.0f, 24.0f),
		glm::vec3(-42.0f, 61.0f, 24.0f),

		// Ninth Post
		glm::vec3(-47.0f, 48.0f, 24.0f),
		glm::vec3(-47.0f, 49.0f, 24.0f),
		glm::vec3(-47.0f, 50.0f, 24.0f),
		glm::vec3(-47.0f, 51.0f, 24.0f),
		glm::vec3(-47.0f, 52.0f, 24.0f),
		glm::vec3(-47.0f, 53.0f, 24.0f),
		glm::vec3(-47.0f, 54.0f, 24.0f),
		glm::vec3(-47.0f, 55.0f, 24.0f),
		glm::vec3(-47.0f, 56.0f, 24.0f),
		glm::vec3(-47.0f, 57.0f, 24.0f),
		glm::vec3(-47.0f, 58.0f, 24.0f),
		glm::vec3(-47.0f, 59.0f, 24.0f),
		glm::vec3(-47.0f, 60.0f, 24.0f),
		glm::vec3(-47.0f, 61.0f, 24.0f),
		glm::vec3(-47.0f, 62.0f, 24.0f),
		glm::vec3(-47.0f, 63.0f, 24.0f),
		glm::vec3(-47.0f, 64.0f, 24.0f),
		glm::vec3(-47.0f, 65.0f, 24.0f),
		glm::vec3(-47.0f, 66.0f, 24.0f)
	};
	auto numVertRails = std::size(vertRailPositions);
	
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	ShapeData sphere = ShapeGenerator::makeSphere();

	GLsizeiptr currentOffset = 0;
	unsigned int sphereVBO{}, sphereVAO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);
	currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	sphereNumIndices = sphere.numIndices;
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO);

	ShapeData sphere2 = ShapeGenerator::makeSphere();
	unsigned int sphereVBO2{}, sphereVAO2;
	glGenVertexArrays(1, &sphereVAO2);
	glGenBuffers(1, &sphereVBO2);
	glBindVertexArray(sphereVAO2);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO2);
	glBufferData(GL_ARRAY_BUFFER, sphere2.vertexBufferSize() + sphere2.indexBufferSize(), 0, GL_STATIC_DRAW);
	currentOffset = -2;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere2.vertexBufferSize(), sphere2.vertices);
	currentOffset += sphere2.vertexBufferSize();
	sphereIndexByteOffset2 = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere2.indexBufferSize(), sphere2.indices);
	sphereNumIndices2 = sphere2.numIndices;
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO2);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightingShader.setInt("material.specular", 1);
	


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations 
		// Still unable to do orthagonal switch correctly adding glew library messed everything up
		if (perspective) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		}
		else {
			perspective = false;
			projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(0.5f);
		lightingShader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// render containers
		glBindVertexArray(cubeVAO);

		//Stairs Loop
		for (unsigned int i = 0; i < numCubes; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, cubePositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//Banister Loop
		for (unsigned int i = 0; i < numSmallCubes; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));
			model = glm::translate(model, bannCubePositions[i]);

			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//Railing Loop
		for (unsigned int i = 0; i < numRailCubes; i++) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

			model = glm::translate(model, railCubePositions[i]);


			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//Vertical Rail Loop
		for (unsigned int i = 0; i < numVertRails; i++) {
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
			model = glm::translate(model, vertRailPositions[i]);
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(sphereVAO);
		model = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.3f, 2.35f, 2.4f));
		model = glm::scale(model, glm::vec3(0.1f)); // Make it a smaller sphere
		lightingShader.setMat4("model", model);

		// draw sphere
		glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		if (!(perspective)) {
			perspective = true;
		}
		else {
			perspective = false;
		}
}

void UCreatePrism(GLfloat verts[], GLushort indices[], int numSides, float radius, float halfLen)
{
	// create constant for 2PI used in calculations
	const float TWO_PI = 2.0f * 3.1415926f;
	const float radiansPerSide = TWO_PI / numSides;

	// value to increment after each vertex is created
	int currentVertex = 0;

	// in this  algorithm, vertex zero is the top center vertex, and vertex one is the bottom center
	// each is offset by the step size
	verts[0] = 0.0f;    // 0 x
	verts[1] = halfLen; // 0 y
	verts[2] = 0.0f;    // 0 z
	verts[3] = 1.0f;    // 0 r
	verts[4] = 0.0f;    // 0 g
	verts[5] = 0.0f;    // 0 b
	verts[6] = 1.0f;    // 0 a
	currentVertex++;

	verts[7] = 0.0f;    // 1 x
	verts[8] = -halfLen;// 1 y
	verts[9] = 0.0f;    // 1 z
	verts[10] = 1.0f;   // 1 r
	verts[11] = 0.0f;   // 1 g
	verts[12] = 0.0f;   // 1 b
	verts[13] = 1.0f;   // 1 a
	currentVertex++;

	// variable to keep track of every triangle added to indices
	int currentTriangle = 0;

	// note: the number of flat sides is equal to the number of edge on the sides
	for (int edge = 0; edge < numSides; edge++)
	{
		// calculate theta, which is the angle from the center point to the next vertex
		float theta = ((float)edge) * radiansPerSide;

		// top triangle first perimeter vertex
		verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
		verts[currentVertex * STRIDE + 1] = halfLen;                // y
		verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
		verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
		verts[currentVertex * STRIDE + 4] = 1.0f;                   // g
		verts[currentVertex * STRIDE + 5] = 0.0f;                   // b
		verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
		currentVertex++;

		// bottom triangle first perimeter vertex
		verts[currentVertex * STRIDE + 0] = radius * cos(theta);    // x
		verts[currentVertex * STRIDE + 1] = -halfLen;               // y
		verts[currentVertex * STRIDE + 2] = radius * sin(theta);    // z
		verts[currentVertex * STRIDE + 3] = 0.0f;                   // r
		verts[currentVertex * STRIDE + 4] = 0.0f;                   // g
		verts[currentVertex * STRIDE + 5] = 1.0f;                   // b
		verts[currentVertex * STRIDE + 6] = 1.0f;                   // a
		currentVertex++;

		if (edge > 0)
		{
			// now to create the indices for the triangles
			// top triangle
			indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
			indices[(3 * currentTriangle) + 1] = currentVertex - 4; // upper left vertex of side
			indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper right vertex of side
			currentTriangle++;

			// bottom triangle
			indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
			indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
			indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
			currentTriangle++;

			// triangle for 1/2 retangular side
			indices[(3 * currentTriangle) + 0] = currentVertex - 4; // upper left vertex of side
			indices[(3 * currentTriangle) + 1] = currentVertex - 3; // bottom left vertex of side
			indices[(3 * currentTriangle) + 2] = currentVertex - 1; // bottom right vertex of side
			currentTriangle++;

			// triangle for second 1/2 retangular side
			indices[(3 * currentTriangle) + 0] = currentVertex - 1; // bottom right vertex of side
			indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper right vertex of side
			indices[(3 * currentTriangle) + 2] = currentVertex - 4; // upper left vertex of side
			currentTriangle++;

		}

	}

	// now, just need to wire up the last side
	// now to create the indices for the triangles
	// top triangle
	indices[(3 * currentTriangle) + 0] = 0;                 // center of top of prism
	indices[(3 * currentTriangle) + 1] = currentVertex - 2; // upper left vertex of side
	indices[(3 * currentTriangle) + 2] = 2;                 // first upper left vertex created, now right
	currentTriangle++;

	// bottom triangle
	indices[(3 * currentTriangle) + 0] = 1;                 // center of bottom of prism
	indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
	indices[(3 * currentTriangle) + 2] = 3;                 // first bottom left vertex created, now right
	currentTriangle++;

	// triangle for 1/2 retangular side
	indices[(3 * currentTriangle) + 0] = currentVertex - 2; // upper left vertex of side
	indices[(3 * currentTriangle) + 1] = currentVertex - 1; // bottom left vertex of side
	indices[(3 * currentTriangle) + 2] = 3;                 // bottom right vertex of side
	currentTriangle++;

	// triangle for second 1/2 retangular side
	indices[(3 * currentTriangle) + 0] = 3;                 // bottom right vertex of side
	indices[(3 * currentTriangle) + 1] = 2;                 // upper right vertex of side
	indices[(3 * currentTriangle) + 2] = currentVertex - 2; // upper left vertex of side
	currentTriangle++;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}