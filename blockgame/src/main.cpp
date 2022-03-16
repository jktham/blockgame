#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <streambuf>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

#include "global.h"
#include "stb_image.h"
#include "game.h"
#include "perlin.h"
#include "terrain.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "light.h"
#include "ui.h"

// callbacks
void processInputState(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	game = new Game;
	terrain = new Terrain;
	//world = new World;
	//player = new Player;
	//camera = new Camera;
	light = new Light;
	ui = new UI;

	// window setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test window", NULL, NULL);
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
	data = stbi_load("res/textures/atlas.png", &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(0);

	unsigned int font_texture;
	glGenTextures(1, &font_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	data = stbi_load("res/fonts/arial.png", &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glActiveTexture(0);

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
	glUniform1i(glGetUniformLocation(ui_shader, "font_texture"), 1);
	glUseProgram(0);

	// world matrices
	glm::mat4 world_model = glm::mat4(1.0f);
	glm::mat4 world_view = glm::mat4(1.0f);
	glm::mat4 world_projection = glm::perspective(glm::radians(80.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// setup
	ui->generateFont();
	ui->createMenu();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		if ((float)glfwGetTime() - last_frame >= 1.0f / FRAME_RATE_LIMIT)
		{
			// timing
			current_frame = (float)glfwGetTime();
			delta_time = current_frame - last_frame;
			last_frame = current_frame;

			if (past_frames.size() >= 10)
			{
				past_frames.pop_front();
			}
			past_frames.push_back(delta_time);
			frame_rate = 10.0f / std::accumulate(past_frames.begin(), past_frames.end(), 0.0f);

			std::cout << std::fixed;
			if (game->state == State::MENU)
			{
				std::cout << "pos: (" << last_x << ", " << last_y << "),  ";
			}
			else if (game->state == State::ALIVE)
			{
				std::cout << std::setprecision(2);
				std::cout << "pos: (" << player->position.x << ", " << player->position.y << ", " << player->position.z << "),  ";
				std::cout << "chunk: (" << player->current_chunk.x << ", " << player->current_chunk.y << "),  ";
				std::cout << "vert: " << player->vertical_velocity << ",  ";
			}
			std::cout << std::setprecision(4);
			std::cout << "delta: " << delta_time << ",  ";
			std::cout << std::setprecision(2);
			std::cout << "fps: " << frame_rate << "\n";

			// clear buffers
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// process current input state
			processInputState(window);

			if (game->state == State::MENU)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

				ui->updateMenu();

				glUseProgram(ui_shader);
				glUniformMatrix4fv(glGetUniformLocation(ui_shader, "model"), 1, GL_FALSE, glm::value_ptr(ui_model));
				glUniformMatrix4fv(glGetUniformLocation(ui_shader, "view"), 1, GL_FALSE, glm::value_ptr(ui_view));
				glUniformMatrix4fv(glGetUniformLocation(ui_shader, "projection"), 1, GL_FALSE, glm::value_ptr(ui_projection));
				glUseProgram(0);

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				glUseProgram(ui_shader);
				glBindVertexArray(ui_VAO);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)ui->mesh.size() / 8);
				glBindVertexArray(0);
				glUseProgram(0);

				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}
			else if (game->state == State::ALIVE)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				// update
				player->applyGravity();
				camera->updatePosition();
				light->update();
				world->updateChunks();
				ui->updateHud();

				glm::vec3 selected_block = glm::floor(camera->getRayIntersect());

				world_view = camera->getViewMatrix();
				world_projection = camera->getProjectionMatrix();

				// set uniforms
				glUseProgram(world_shader);
				glUniformMatrix4fv(glGetUniformLocation(world_shader, "model"), 1, GL_FALSE, glm::value_ptr(world_model));
				glUniformMatrix4fv(glGetUniformLocation(world_shader, "view"), 1, GL_FALSE, glm::value_ptr(world_view));
				glUniformMatrix4fv(glGetUniformLocation(world_shader, "projection"), 1, GL_FALSE, glm::value_ptr(world_projection));
				glUniform3f(glGetUniformLocation(world_shader, "light.direction"), light->direction.x, light->direction.y, light->direction.z);
				glUniform3f(glGetUniformLocation(world_shader, "light.color"), light->color.x, light->color.y, light->color.z);
				glUniform3f(glGetUniformLocation(world_shader, "light.ambient"), light->ambient.x, light->ambient.y, light->ambient.z);
				glUniform3f(glGetUniformLocation(world_shader, "light.diffuse"), light->diffuse.x, light->diffuse.y, light->diffuse.z);
				glUniform3f(glGetUniformLocation(world_shader, "light.specular"), light->specular.x, light->specular.y, light->specular.z);
				glUniform3f(glGetUniformLocation(world_shader, "view_pos"), camera->position.x, camera->position.y, camera->position.z);
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
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)world->mesh.size() / 11);
				glBindVertexArray(0);
				glUseProgram(0);

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				glUseProgram(ui_shader);
				glBindVertexArray(ui_VAO);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)ui->mesh.size() / 8);
				glBindVertexArray(0);
				glUseProgram(0);

				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}

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
	if (game->state == State::MENU)
	{
	}
	else if (game->state == State::ALIVE)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			player->processAction(Action::MOVE_FRONT);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			player->processAction(Action::MOVE_BACK);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			player->processAction(Action::MOVE_LEFT);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			player->processAction(Action::MOVE_RIGHT);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			player->sprint = true;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
			player->sprint = false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		{
			if (left_delay <= 0)
			{
				player->processAction(Action::BLOCK_DESTROY);
				left_delay = CLICK_DELAY;
			}
			left_delay -= 1;
		}
		else
		{
			left_delay = 0;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			if (right_delay <= 0)
			{
				player->processAction(Action::BLOCK_PLACE);
				right_delay = CLICK_DELAY;
			}
			right_delay -= 1;
		}
		else
		{
			right_delay = 0;
		}
	}
}

// discrete inputs
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		wireframe_mode = !wireframe_mode;
		if (wireframe_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (game->state == State::MENU)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			game->start();
	}
	else if (game->state == State::ALIVE)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			game->quit();

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			player->processAction(Action::MOVE_JUMP);

		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
			player->noclip = !player->noclip;

		if (key == GLFW_KEY_K && action == GLFW_PRESS)
			player->processAction(Action::BLOCK_PICK);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (game->state == State::MENU)
	{
	}
	else if (game->state == State::ALIVE)
	{
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
			player->processAction(Action::BLOCK_PICK);
	}
}

void mouse_cursor_callback(GLFWwindow* window, double pos_x, double pos_y)
{
	if (game->state == State::MENU)
	{
		last_x = (float)pos_x;
		last_y = (float)pos_y;
	}
	else if (game->state == State::ALIVE)
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

		player->processMouseMovement(offset_x, offset_y);
	}
}

void mouse_scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
	if (game->state == State::MENU)
	{
	}
	else if (game->state == State::ALIVE)
	{
		current_type -= (int)offset_y;

		if (current_type > MAX_TYPE)
			current_type = 1;
		if (current_type < 1)
			current_type = MAX_TYPE;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}