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
#include <chrono>

#include "global.h"
#include "stb_image.h"
#include "perlin.h"
#include "camera.h"
#include "world.h"
#include "light.h"
#include "ui.h"

// classes
Camera camera;
World world;
Light light;
UI ui;

// callbacks
void processInputState(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// buffers
	glGenVertexArrays(1, &world_VAO);
	glGenBuffers(1, &world_VBO);

	glGenVertexArrays(1, &ui_VAO);
	glGenBuffers(1, &ui_VBO);

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

	// world vertex shader
	const char* world_vert_source;

	std::ifstream world_vert_file("src/world_shader.vs");
	std::string world_vert_string((std::istreambuf_iterator<char>(world_vert_file)), std::istreambuf_iterator<char>());
	world_vert_source = world_vert_string.c_str();

	unsigned int world_vert_shader;
	world_vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(world_vert_shader, 1, &world_vert_source, NULL);
	glCompileShader(world_vert_shader);

	// world fragment shader
	const char* world_frag_source;

	std::ifstream world_frag_file("src/world_shader.fs");
	std::string world_frag_string((std::istreambuf_iterator<char>(world_frag_file)), std::istreambuf_iterator<char>());
	world_frag_source = world_frag_string.c_str();

	unsigned int world_frag_shader;
	world_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(world_frag_shader, 1, &world_frag_source, NULL);
	glCompileShader(world_frag_shader);

	// world shader program
	unsigned int world_shader;
	world_shader = glCreateProgram();

	glAttachShader(world_shader, world_vert_shader);
	glAttachShader(world_shader, world_frag_shader);
	glLinkProgram(world_shader);

	glDeleteShader(world_vert_shader);
	glDeleteShader(world_frag_shader);

	// ui vertex shader
	const char* ui_vert_source;

	std::ifstream ui_vert_file("src/ui_shader.vs");
	std::string ui_vert_string((std::istreambuf_iterator<char>(ui_vert_file)), std::istreambuf_iterator<char>());
	ui_vert_source = ui_vert_string.c_str();

	unsigned int ui_vert_shader;
	ui_vert_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(ui_vert_shader, 1, &ui_vert_source, NULL);
	glCompileShader(ui_vert_shader);

	// ui fragment shader
	const char* ui_frag_source;

	std::ifstream ui_frag_file("src/ui_shader.fs");
	std::string ui_frag_string((std::istreambuf_iterator<char>(ui_frag_file)), std::istreambuf_iterator<char>());
	ui_frag_source = ui_frag_string.c_str();

	unsigned int ui_frag_shader;
	ui_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(ui_frag_shader, 1, &ui_frag_source, NULL);
	glCompileShader(ui_frag_shader);

	// ui shader program
	unsigned int ui_shader;
	ui_shader = glCreateProgram();

	glAttachShader(ui_shader, ui_vert_shader);
	glAttachShader(ui_shader, ui_frag_shader);
	glLinkProgram(ui_shader);

	glDeleteShader(ui_vert_shader);
	glDeleteShader(ui_frag_shader);

	// world texture uniforms
	glUseProgram(world_shader);
	glUniform1i(glGetUniformLocation(world_shader, "atlas_texture"), 0);
	glUseProgram(0);

	// ui texture uniforms
	glUseProgram(ui_shader);
	glUniform1i(glGetUniformLocation(ui_shader, "atlas_texture"), 0);
	glUseProgram(0);

	// world matrices
	glm::mat4 world_model = glm::mat4(1.0f);
	glm::mat4 world_view = camera.getViewMatrix();
	glm::mat4 world_projection = glm::perspective(glm::radians(camera.m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	// ui matrices
	glm::mat4 ui_model = glm::mat4(1.0f);
	glm::mat4 ui_view = glm::mat4(1.0f);
	glm::mat4 ui_projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f);

	// opengl settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEBUG_OUTPUT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// world setup
	world.createChunks();
	world.generateChunkMesh();
	world.generateWorldMesh();
	world.updateVAO();

	// ui setup
	ui.generateHudMesh();
	ui.updateVAO();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		if ((float)glfwGetTime() - last_frame > 1.0f / FRAME_RATE_LIMIT)
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

			// process current input state
			processInputState(window);

			// update
			camera.applyGravity();
			light.update();
			world.updateChunks();
			ui.updateHud();

			glm::vec3 selected_block = std::get<0>(camera.getRayIntersect());

			world_view = camera.getViewMatrix();
			world_projection = camera.getProjectionMatrix();

			// set uniforms
			glUseProgram(world_shader);
			glUniformMatrix4fv(glGetUniformLocation(world_shader, "model"), 1, GL_FALSE, glm::value_ptr(world_model));
			glUniformMatrix4fv(glGetUniformLocation(world_shader, "view"), 1, GL_FALSE, glm::value_ptr(world_view));
			glUniformMatrix4fv(glGetUniformLocation(world_shader, "projection"), 1, GL_FALSE, glm::value_ptr(world_projection));
			glUniform3f(glGetUniformLocation(world_shader, "light.direction"), light.m_direction.x, light.m_direction.y, light.m_direction.z);
			glUniform3f(glGetUniformLocation(world_shader, "light.color"), light.m_color.x, light.m_color.y, light.m_color.z);
			glUniform3f(glGetUniformLocation(world_shader, "light.ambient"), light.m_ambient.x, light.m_ambient.y, light.m_ambient.z);
			glUniform3f(glGetUniformLocation(world_shader, "light.diffuse"), light.m_diffuse.x, light.m_diffuse.y, light.m_diffuse.z);
			glUniform3f(glGetUniformLocation(world_shader, "light.specular"), light.m_specular.x, light.m_specular.y, light.m_specular.z);
			glUniform3f(glGetUniformLocation(world_shader, "view_pos"), camera.m_position.x, camera.m_position.y, camera.m_position.z);
			glUniform3f(glGetUniformLocation(world_shader, "selected_block"), selected_block.x, selected_block.y, selected_block.z);
			glUseProgram(0);

			glUseProgram(ui_shader);
			glUniformMatrix4fv(glGetUniformLocation(ui_shader, "model"), 1, GL_FALSE, glm::value_ptr(ui_model));
			glUniformMatrix4fv(glGetUniformLocation(ui_shader, "view"), 1, GL_FALSE, glm::value_ptr(ui_view));
			glUniformMatrix4fv(glGetUniformLocation(ui_shader, "projection"), 1, GL_FALSE, glm::value_ptr(ui_projection));
			glUseProgram(0);

			// draw vertices
			glUseProgram(world_shader);
			glBindVertexArray(world_VAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)world.m_mesh.size() / 11);
			glBindVertexArray(0);
			glUseProgram(0);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			glUseProgram(ui_shader);
			glBindVertexArray(ui_VAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)ui.m_mesh.size() / 7);
			glBindVertexArray(0);
			glUseProgram(0);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glDeleteVertexArrays(1, &world_VAO);
	glDeleteBuffers(1, &world_VBO);
	glDeleteVertexArrays(1, &ui_VAO);
	glDeleteBuffers(1, &ui_VBO);
	glfwTerminate();
	return 0;
}

// continuous inputs
void processInputState(GLFWwindow* window)
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (left_delay <= 0)
		{
			world.destroyBlock(camera.getRayIntersect());
			left_delay = CLICK_DELAY;
		}
		left_delay -= 1;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		left_delay = 0;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (right_delay <= 0)
		{
			world.placeBlock(camera.getRayIntersect());
			right_delay = CLICK_DELAY;
		}
		right_delay -= 1;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		right_delay = 0;
}

// discrete inputs
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		current_type = world.getBlockType(camera.getRayIntersect());
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

void mouse_scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
	current_type -= (int)offset_y;

	if (current_type > MAX_TYPE)
		current_type = 1;
	if (current_type < 1)
		current_type = MAX_TYPE;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}