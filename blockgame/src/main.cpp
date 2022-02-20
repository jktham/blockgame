#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <iomanip>
#include <vector>

#include "global.h"
#include "stb_image.h"
#include "perlin.h"
#include "camera.h"
#include "world.h"
#include "light.h"

// classes
Camera camera;
World world;
Light light;

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
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
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// load textures
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data;

	unsigned int atlas_texture;
	glGenTextures(1, &atlas_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas_texture);
	data = stbi_load("res/atlas.png", &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// texture attributes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// vertex shader
	const char* vert_source;

	std::ifstream vert_file("src/shader.vs");
	std::string vert_string((std::istreambuf_iterator<char>(vert_file)), std::istreambuf_iterator<char>());
	vert_source = vert_string.c_str();

	unsigned int vert_shader;
	vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vert_shader, 1, &vert_source, NULL);
	glCompileShader(vert_shader);

	// fragment shader
	const char* frag_source;

	std::ifstream frag_file("src/shader.fs");
	std::string frag_string((std::istreambuf_iterator<char>(frag_file)), std::istreambuf_iterator<char>());
	frag_source = frag_string.c_str();

	unsigned int frag_shader;
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(frag_shader, 1, &frag_source, NULL);
	glCompileShader(frag_shader);

	// shader program
	unsigned int shader;
	shader = glCreateProgram();

	glAttachShader(shader, vert_shader);
	glAttachShader(shader, frag_shader);
	glLinkProgram(shader);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	// texture uniforms
	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "atlas_texture"), 0);
	glUseProgram(0);

	// matrices
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// opengl settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEBUG_OUTPUT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// world setup
	world.createChunks();
	world.generateMesh();
	world.updateMesh();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		if ((float)glfwGetTime() - last_frame > 1.0f / frame_rate_limit)
		{
			// timing
			current_frame = (float)glfwGetTime();
			delta_time = current_frame - last_frame;
			last_frame = current_frame;

			std::cout << std::fixed;
			std::cout << std::setprecision(2);
			std::cout << "pos: (" << camera.m_position.x << ", " << camera.m_position.y << ", " << camera.m_position.z << "),  ";
			std::cout << "chunk: (" << current_chunk.x << ", " << current_chunk.y << "),  ";
			std::cout << "vert: " << camera.m_vertical_velocity << ",  ";
			std::cout << std::setprecision(4);
			std::cout << "delta: " << delta_time << ",  ";
			std::cout << std::setprecision(2);
			std::cout << "fps: " << 1.0f / delta_time << "\n";

			// clear buffers
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// keyboard input
			processInput(window);

			// update
			camera.applyGravity();
			light.update();
			world.shiftChunks();

			// update matrices
			model = glm::mat4(1.0f);
			view = camera.getViewMatrix();
			projection = glm::perspective(glm::radians(camera.m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);

			// set uniforms
			glUseProgram(shader);
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform3f(glGetUniformLocation(shader, "light.direction"), light.m_direction.x, light.m_direction.y, light.m_direction.z);
			glUniform3f(glGetUniformLocation(shader, "light.color"), light.m_color.x, light.m_color.y, light.m_color.z);
			glUniform3f(glGetUniformLocation(shader, "light.ambient"), light.m_ambient.x, light.m_ambient.y, light.m_ambient.z);
			glUniform3f(glGetUniformLocation(shader, "light.diffuse"), light.m_diffuse.x, light.m_diffuse.y, light.m_diffuse.z);
			glUniform3f(glGetUniformLocation(shader, "light.specular"), light.m_specular.x, light.m_specular.y, light.m_specular.z);
			glUniform3f(glGetUniformLocation(shader, "view_pos"), camera.m_position.x, camera.m_position.y, camera.m_position.z);

			// draw vertices
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)world.m_mesh.size() / 3);

			glBindVertexArray(0);
			glUseProgram(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

// discrete key press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		camera.processKeyboard(GLFW_KEY_SPACE);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		wireframe_mode = !wireframe_mode;

		if (wireframe_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		camera.m_noclip = !camera.m_noclip;
}

// continuous key press
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(GLFW_KEY_W);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(GLFW_KEY_S);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(GLFW_KEY_A);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(GLFW_KEY_D);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.m_speed = 7.5f * 5.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.m_speed = 7.5f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double pos_x, double pos_y)
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		world.destroyBlock(camera.getRayIntersect());
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		world.placeBlock(camera.getRayIntersect());
}