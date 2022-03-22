﻿#include "game.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "global.h"

#include <glm/glm.hpp>

#include <vector>

void Game::start()
{
	state = State::ALIVE;
	first_mouse = true;

	world = new World;
	player = new Player;
	camera = new Camera;

	world->createChunks();
	world->generateChunkMesh();
	world->generateWorldMesh();
	world->updateExposedBlocks();
	world->updateVAO(world->complete_mesh);
}

void Game::quit()
{
	state = State::MENU;

	delete world;
	delete player;
	delete camera;
}