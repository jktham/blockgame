#include "game.h"
#include "terrain.h"
#include "world.h"
#include "inventory.h"
#include "player.h"
#include "camera.h"
#include "global.h"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

void Game::start()
{
	state = State::ALIVE;
	first_mouse = true;
	first_frame = true;

	terrain = new Terrain;
	world = new World;
	inventory = new Inventory;
	player = new Player;
	camera = new Camera;

	world->initializeChunks();
	world->generateChunkMesh();
	world->generateWorldMesh();
	world->updateExposedBlocks();
	world->updateVAO(world->complete_mesh);

	inventory->initializeItems();
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

void Game::save(std::string path)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	std::string output;

	output.append("inventory");
	output.append("\n");
	for (int i = 0; i < inventory->slots.size(); i++)
	{
		output.append(std::to_string(inventory->slots[i].id));
		output.append(",");
		output.append(std::to_string(inventory->slots[i].amount));
		output.append("\n");
	}

	output.append("terrain");
	output.append("\n");
	output.append(std::to_string(terrain->OFFSET));
	output.append("\n");
	output.append(std::to_string(terrain->FREQUENCY));
	output.append("\n");
	output.append(std::to_string(terrain->AMPLITUDE));
	output.append("\n");
	output.append(std::to_string(terrain->OCTAVES));
	output.append("\n");
	output.append(std::to_string(terrain->SHIFT));
	output.append("\n");
	output.append(std::to_string(terrain->LACUNARITY));
	output.append("\n");
	output.append(std::to_string(terrain->PERSISTENCE));
	output.append("\n");

	output.append("world");
	output.append("\n");
	for (int i = 0; i < world->changes.size(); i++)
	{
		output.append(std::to_string((int)world->changes[i].pos.x));
		output.append(",");
		output.append(std::to_string((int)world->changes[i].pos.y));
		output.append(",");
		output.append(std::to_string((int)world->changes[i].pos.z));
		output.append(",");
		output.append(std::to_string(world->changes[i].type));
		output.append("\n");
	}

	std::fstream file(path, std::ios_base::trunc | std::ios_base::out);
	file << output;
	file.close();

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "saved changes: (" << world->changes.size() << "), " << ms_int << "\n";
}

void Game::load(std::string path)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	first_frame = true;

	std::fstream file(path, std::fstream::in);
	std::vector<std::string> lines;
	std::string line;

	world->changes = {};

	while (std::getline(file, line, '\n'))
	{
		lines.push_back(line);
	}

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i] == "inventory")
		{
			for (int j = 0; j < inventory->slots.size(); j++)
			{
				std::vector<float> values;
				std::stringstream iss_line(lines[i + j + 1]);
				std::string value;
				while (std::getline(iss_line, value, ',')) {
					values.push_back(stof(value));
				}
				inventory->slots[j].id = (int)values[0];
				inventory->slots[j].amount = (int)values[1];
			}
		}
		if (lines[i] == "terrain")
		{
			std::vector<float> values;
			for (int j = 0; j < 7; j++)
			{
				values.push_back(stof((lines[i + j + 1])));
			}
			terrain->OFFSET = (int)values[0];
			terrain->FREQUENCY = values[1];
			terrain->AMPLITUDE = values[2];
			terrain->OCTAVES = (int)values[3];
			terrain->SHIFT = (int)values[4];
			terrain->LACUNARITY = values[5];
			terrain->PERSISTENCE = values[6];
		}
		if (lines[i] == "world")
		{
			for (int j = 0; j < lines.size() - i - 1; j++)
			{
				std::stringstream iss_line(lines[i + j + 1]);
				std::string value;
				std::vector<float> values;
				while (std::getline(iss_line, value, ','))
				{
					values.push_back(std::stof(value));
				}
				world->changes.push_back(Change(glm::vec3((int)values[0], (int)values[1], (int)values[2]), (int)values[3]));
			}
		}
	}

	file.close();

	for (int m = 0; m < WORLD_SIZE.x; m++)
	{
		for (int n = 0; n < WORLD_SIZE.y; n++)
		{
			world->chunks[m][n].generateChunk();
		}
	}

	world->generateChunkMesh();
	world->generateWorldMesh();
	world->updateExposedBlocks();
	world->updateVAO(world->complete_mesh);

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "loaded changes: (" << world->changes.size() << "), " << ms_int << "\n";
}