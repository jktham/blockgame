#include "game.h"
#include "terrain.h"
#include "world.h"
#include "inventory.h"
#include "player.h"
#include "camera.h"
#include "global.h"

#include <glm/glm.hpp>

#include <vector>

void Game::start()
{
	state = State::ALIVE;
	first_mouse = true;

	terrain = new Terrain;
	world = new World;
	inventory = new Inventory;
	player = new Player;
	camera = new Camera;

	world->createChunks();
	world->generateChunkMesh();
	world->generateWorldMesh();
	world->updateExposedBlocks();
	world->updateVAO(world->complete_mesh);

	inventory->createItems();
	inventory->giveItem(5, 64);
}

void Game::quit()
{
	state = State::MENU;

	delete terrain;
	delete world;
	delete inventory;
	delete player;
	delete camera;
}