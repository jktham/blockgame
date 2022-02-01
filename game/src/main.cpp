#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <iomanip>

#include "stb_image.h"
#include "shader.h"
#include "camera.h"

// settings
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

// camera
Camera camera(glm::vec3(10.0f, 4.0f, 10.0f));
float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
float frame_rate = 0.0f;
float frame_rate_limit = 120.0f;

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
	// window setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test window", NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// vertex data
	float vertices[] = {
	 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

	 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
	};

	const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 step = glm::vec3(1.0f, -10.0f, 1.0f);
	const glm::vec3 size = glm::vec3(20, 1, 20);

	std::vector<float> offsets;

	for (int i = 0; i < size.x; i++)
	{
		for (int j = 0; j < size.y; j++)
		{
			for (int k = 0; k < size.z; k++)
			{
				offsets.push_back(origin.x + i * step.x);
				offsets.push_back(origin.y + j * step.y);
				offsets.push_back(origin.z + k * step.z);
			}
		}
	}

	// vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// vertex buffer object
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// instance vertex buffer object
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * (GLsizei)(size.x * size.y * size.z), &offsets[0], GL_STATIC_DRAW);
	glVertexAttribDivisor(1, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// texture attributes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load textures
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data;

	unsigned int dirt_texture;
	glGenTextures(1, &dirt_texture);
	glBindTexture(GL_TEXTURE_2D, dirt_texture);
	data = stbi_load("res/dirt.jpg", &width, &height, &channels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	// vertex attribute (position)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (instance offsets)
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (texture)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// shader program
	Shader shader("src/shader.vs", "src/shader.fs");

	// set texture uniforms
	shader.use();
	shader.setInt("dirt_texture", 0);

	// matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// opengl settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		if ((float)glfwGetTime() - last_frame > 1.0f / frame_rate_limit)
		{
			// timing
			current_frame = (float)glfwGetTime();
			delta_time = current_frame - last_frame;
			last_frame = current_frame;
			frame_rate = 1.0f / delta_time;

			std::cout << std::fixed;
			std::cout << std::setprecision(4);
			std::cout << "time: " << (float)glfwGetTime() << ", delta: " << delta_time << ", fps: " << frame_rate << "\n";

			// keyboard input
			processInput(window);

			camera.applyGravity(delta_time);

			// clear buffers
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// update matrices
			model = glm::mat4(1.0f);
			view = camera.getViewMatrix();
			projection = glm::perspective(glm::radians(camera.m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

			// set uniforms
			shader.use();
			shader.setMat4("model", model);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			// bind textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, dirt_texture);

			// draw vertices
			glBindVertexArray(VAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)(size.x * size.y * size.z));

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::JUMP, delta_time);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float camera_speed = 3.0f * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::RIGHT, delta_time);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.m_speed = SHIFT_SPEED;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.m_speed = SPEED;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double pos_x, double pos_y)
{
	if (first_mouse)
	{
		last_x = (float)pos_x;
		last_y = (float)pos_y;
		first_mouse = false;
	}

	float offset_x = (float)pos_x - last_x;
	float offset_y = last_y - (float)pos_y;
	last_x = (float)pos_x;
	last_y = (float)pos_y;

	camera.processMouseMovement(offset_x, offset_y);
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
	camera.processMouseScroll(static_cast<float>(offset_y));
}