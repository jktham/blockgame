#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#include "global.h"
#include "game.h"
#include "world.h"
#include "player.h"
#include "camera.h"

void Game::start()
{
	state = State::ALIVE;
	first_mouse = true;

	world = new World;
	world->createChunks();
	world->generateChunkMesh();
	world->generateWorldMesh();
	world->updateVAO();

	player = new Player;
	camera = new Camera;
	current_chunk = glm::vec2(0.0f, 0.0f);
	last_chunk = glm::vec2(0.0f, 0.0f);
}

void Game::quit()
{
	state = State::MENU;
	current_type = MAX_TYPE;
	last_type = -1;

	delete world;
	delete player;
	delete camera;
}