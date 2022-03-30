#include "world.h"
#include "terrain.h"
#include "inventory.h"
#include "global.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

void Chunk::generateChunk()
{
	generateTerrain();

	for (int i = 0; i < world->changes.size(); i++)
	{
		if (world->changes[i].pos.x >= chunk_pos.x && world->changes[i].pos.x < chunk_pos.x + CHUNK_SIZE.x && world->changes[i].pos.y >= chunk_pos.y && world->changes[i].pos.y < chunk_pos.y + CHUNK_SIZE.y)
		{
			int x = (int)(world->changes[i].pos.x - chunk_pos.x);
			int y = (int)(world->changes[i].pos.y - chunk_pos.y);
			int z = (int)(world->changes[i].pos.z);

			blocks[x][y][z].type = world->changes[i].type;

			if (z - 1 < min_z)
				min_z = z - 1;
		}
	}
}

void Chunk::generateTerrain()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (int x = 0; x < CHUNK_SIZE.x; x++)
	{
		for (int y = 0; y < CHUNK_SIZE.y; y++)
		{
			float ground_height = terrain->getGroundHeight(x + (int)chunk_pos.x, y + (int)chunk_pos.y);

			for (int z = 0; z < CHUNK_SIZE.z; z++)
			{
				if (z == 0)
				{
					blocks[x][y][z].type = 4;
				}
				else if (z < ground_height - 5)
				{
					blocks[x][y][z].type = 3;
				}
				else if (z < ground_height - 1)
				{
					blocks[x][y][z].type = 2;
				}
				else if (z < ground_height)
				{
					blocks[x][y][z].type = 1;
				}
				else
				{
					blocks[x][y][z].type = 0;
				}

				if (blocks[x][y][z].type == 0)
				{
					if (z - 1 < min_z)
					{
						min_z = z - 1;
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "generated terrain: (" << chunk_pos.x << ", " << chunk_pos.y << "), " << ms_int << "\n";
}

void World::initializeChunks()
{
	for (int m = 0; m < WORLD_SIZE.x; m++)
	{
		for (int n = 0; n < WORLD_SIZE.y; n++)
		{
			chunks[m][n].chunk_pos = glm::vec2((m - WORLD_SIZE.x / 2) * CHUNK_SIZE.x, (n - WORLD_SIZE.y / 2) * CHUNK_SIZE.y);
			chunks[m][n].generateChunk();
		}
	}
}

void World::updateChunks(glm::vec2 shift_direction)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	meshgen_done = false;
	shiftChunks(shift_direction);
	generateWorldMesh();
	meshgen_done = true;
	//updateVAO(front_mesh);

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "updated chunks: " << ms_int << "\n";
}

void World::shiftChunks(glm::vec2 shift_direction)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	if (shift_direction.x == 1)
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				if (m == WORLD_SIZE.x - 1)
				{
					chunks[m][n].chunk_pos += glm::vec2(CHUNK_SIZE.x, 0);
					chunks[m][n].generateChunk();
				}
				else
				{
					chunks[m][n] = chunks[m + 1][n];
				}
			}
		}
		generateChunkMesh(WORLD_SIZE.y - 2, WORLD_SIZE.y, 0, WORLD_SIZE.y);
	}
	if (shift_direction.y == 1)
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				if (n == WORLD_SIZE.y - 1)
				{
					chunks[m][n].chunk_pos += glm::vec2(0, CHUNK_SIZE.y);
					chunks[m][n].generateChunk();
				}
				else
				{
					chunks[m][n] = chunks[m][n + 1];
				}
			}
		}
		generateChunkMesh(0, WORLD_SIZE.x, WORLD_SIZE.y - 2, WORLD_SIZE.y);
	}
	if (shift_direction.x == -1)
	{
		for (int m = WORLD_SIZE.x - 1; m >= 0; m--)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				if (m == 0)
				{
					chunks[m][n].chunk_pos -= glm::vec2(CHUNK_SIZE.x, 0);
					chunks[m][n].generateChunk();
				}
				else
				{
					chunks[m][n] = chunks[m - 1][n];
				}
			}
		}
		generateChunkMesh(0, 2, 0, WORLD_SIZE.y);
	}
	if (shift_direction.y == -1)
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = WORLD_SIZE.y - 1; n >= 0; n--)
			{
				if (n == 0)
				{
					chunks[m][n].chunk_pos -= glm::vec2(0, CHUNK_SIZE.y);
					chunks[m][n].generateChunk();
				}
				else
				{
					chunks[m][n] = chunks[m][n - 1];
				}
			}
		}
		generateChunkMesh(0, WORLD_SIZE.x, 0, 2);
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "shifted chunks: (" << shift_direction.x << ", " << shift_direction.y << "), " << ms_int << "\n";
}

void World::generateChunkMesh(int m_start, int m_end, int n_start, int n_end)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (int m = m_start; m < m_end; m++)
	{
		for (int n = n_start; n < n_end; n++)
		{
			chunks[m][n].mesh = {};
			chunks[m][n].exposed_blocks = {};

			int min_z = chunks[m][n].min_z;

			if (m > 0 && chunks[m - 1][n].min_z < chunks[m][n].min_z && chunks[m - 1][n].min_z < min_z)
			{
				min_z = chunks[m - 1][n].min_z;
			}
			if (m < WORLD_SIZE.x - 1 && chunks[m + 1][n].min_z < chunks[m][n].min_z && chunks[m + 1][n].min_z < min_z)
			{
				min_z = chunks[m + 1][n].min_z;
			}
			if (n > 0 && chunks[m][n - 1].min_z < chunks[m][n].min_z && chunks[m][n - 1].min_z < min_z)
			{
				min_z = chunks[m][n - 1].min_z;
			}
			if (n < WORLD_SIZE.y - 1 && chunks[m][n + 1].min_z < chunks[m][n].min_z && chunks[m][n + 1].min_z < min_z)
			{
				min_z = chunks[m][n + 1].min_z;
			}

			if (min_z < 0)
			{
				min_z = 0;
			}

			for (int x = 0; x < CHUNK_SIZE.x; x++)
			{
				for (int y = 0; y < CHUNK_SIZE.y; y++)
				{
					for (int z = min_z; z < CHUNK_SIZE.z; z++)
					{
						bool exposed = false;

						if (chunks[m][n].blocks[x][y][z].type > 0)
						{
							float chunk_x = chunks[m][n].chunk_pos.x;
							float chunk_y = chunks[m][n].chunk_pos.y;
							float atlas_y = ATLAS_SIZE_Y - chunks[m][n].blocks[x][y][z].type;
							float block_x = (float)(chunk_x + x);
							float block_y = (float)(chunk_y + y);
							float block_z = (float)z;

							float vertices[6][66] = {
								// pos.x, pos.y, pos.z, tex.x, tex.y, norm.x, norm.y, norm.z, block.x, block.y, block.z
								// left
								{
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 0.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (1.0f + 0.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 0.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 0.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (0.0f + 0.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 0.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, -1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								},
								// right
								{
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 1.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 1.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (1.0f + 1.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 1.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 1.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (0.0f + 1.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f, 0.0f, 0.0f, block_x, block_y, block_z,
								},
								// front
								{
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 2.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (1.0f + 2.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (1.0f + 2.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (1.0f + 2.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (0.0f + 2.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 2.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, -1.0f, 0.0f, block_x, block_y, block_z,
								},
								// back
								{
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (0.0f + 3.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 3.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (1.0f + 3.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 3.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (0.0f + 3.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (0.0f + 3.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 1.0f, 0.0f, block_x, block_y, block_z,
								},
								// bottom
								{
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 4.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (1.0f + 4.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (1.0f + 4.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (1.0f + 4.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 0.0f + z, (0.0f + 4.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 0.0f + z, (0.0f + 4.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, -1.0f, block_x, block_y, block_z,
								},
								// top
								{
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (0.0f + 5.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (1.0f + 5.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 5.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								1.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (1.0f + 5.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 1.0f + chunk_y + y, 1.0f + z, (0.0f + 5.0f) / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								0.0f + chunk_x + x, 0.0f + chunk_y + y, 1.0f + z, (0.0f + 5.0f) / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 0.0f, 0.0f, 1.0f, block_x, block_y, block_z,
								},
							};

							if (x > 0 && chunks[m][n].blocks[x - 1][y][z].type == 0 || x == 0 && m > 0 && chunks[m - 1][n].blocks[CHUNK_SIZE.x - 1][y][z].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[0][0], &vertices[0][66]);
								exposed = true;
							}

							if (x < CHUNK_SIZE.x - 1 && chunks[m][n].blocks[x + 1][y][z].type == 0 || x == CHUNK_SIZE.x - 1 && m < WORLD_SIZE.x - 1 && chunks[m + 1][n].blocks[0][y][z].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[1][0], &vertices[1][66]);
								exposed = true;
							}

							if (y > 0 && chunks[m][n].blocks[x][y - 1][z].type == 0 || y == 0 && n > 0 && chunks[m][n - 1].blocks[x][CHUNK_SIZE.y - 1][z].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[2][0], &vertices[2][66]);
								exposed = true;
							}

							if (y < CHUNK_SIZE.y - 1 && chunks[m][n].blocks[x][y + 1][z].type == 0 || y == CHUNK_SIZE.y - 1 && n < WORLD_SIZE.y - 1 && chunks[m][n + 1].blocks[x][0][z].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[3][0], &vertices[3][66]);
								exposed = true;
							}

							if (z > 0 && chunks[m][n].blocks[x][y][z - 1].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[4][0], &vertices[4][66]);
								exposed = true;
							}

							if (z == CHUNK_SIZE.z - 1 || chunks[m][n].blocks[x][y][z + 1].type == 0)
							{
								chunks[m][n].mesh.insert(chunks[m][n].mesh.end(), &vertices[5][0], &vertices[5][66]);
								exposed = true;
							}

							if (exposed)
							{
								chunks[m][n].exposed_blocks.push_back(glm::ivec3(chunks[m][n].chunk_pos.x + x, chunks[m][n].chunk_pos.y + y, z));
							}
						}
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "generated chunk mesh: " << (m_end - m_start) * (n_end - n_start) << " chunks, " << ms_int << "\n";
}

void World::generateWorldMesh()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	mesh = {};

	for (int m = 0; m < WORLD_SIZE.x; m++)
	{
		for (int n = 0; n < WORLD_SIZE.y; n++)
		{
			mesh.insert(mesh.end(), chunks[m][n].mesh.begin(), chunks[m][n].mesh.end());
		}
	}

	complete_mesh = mesh;

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "generated world mesh: " << WORLD_SIZE.x * WORLD_SIZE.y << " chunks, " << ms_int << "\n";
}

void World::updateExposedBlocks()
{
	exposed_blocks = {};

	for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
	{
		for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
		{
			if (m >= 0 && m < WORLD_SIZE.x && n >= 0 && n < WORLD_SIZE.y)
			{
				exposed_blocks.insert(exposed_blocks.end(), chunks[m][n].exposed_blocks.begin(), chunks[m][n].exposed_blocks.end());
			}
		}
	}
}

void World::updateVAO(std::vector<float> data)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	// vertex array object
	glBindVertexArray(world_VAO);

	// vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (position)
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (texture)
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (normal)
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (block)
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "updated VAO: " << data.size() << " verts, " << ms_int << "\n";
}

void World::placeBlock(glm::vec3 position, int type)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	glm::vec3 pos = glm::floor(position);
	glm::vec3 offset = position - glm::floor(position) - glm::vec3(0.5f);

	if (abs(offset.x) > abs(offset.y) && abs(offset.x) > abs(offset.z))
	{
		pos.x += (int)glm::sign(offset.x);
	}
	if (abs(offset.y) > abs(offset.z) && abs(offset.y) > abs(offset.x))
	{
		pos.y += (int)glm::sign(offset.y);
	}
	if (abs(offset.z) > abs(offset.x) && abs(offset.z) > abs(offset.y))
	{
		pos.z += (int)glm::sign(offset.z);
	}

	if (pos.z >= 0 && pos.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type != 4)
				{
					if (pos.x >= chunks[m][n].chunk_pos.x && pos.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && pos.y >= chunks[m][n].chunk_pos.y && pos.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
					{
						inventory->removeItem(type);

						chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type = type;
						changes.push_back(Change(pos, type));

						generateChunkMesh(m - 1, m + 2, n - 1, n + 2);
						generateWorldMesh();
						updateExposedBlocks();
						updateVAO(complete_mesh);
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "placed block: (" << (int)pos.x << ", " << (int)pos.y << ", " << (int)pos.z << "), " << type << ", " << ms_int << "\n";
}

void World::breakBlock(glm::vec3 position)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	glm::vec3 pos = glm::floor(position);

	if (pos.z >= 0 && pos.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (pos.x >= chunks[m][n].chunk_pos.x && pos.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && pos.y >= chunks[m][n].chunk_pos.y && pos.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
				{
					if (chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type != 4)
					{
						inventory->giveItem(chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type, 1);

						chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type = 0;
						changes.push_back(Change(pos, 0));

						if (pos.z - 1 < chunks[m][n].min_z)
							chunks[m][n].min_z = (int)pos.z - 1;

						generateChunkMesh(m - 1, m + 2, n - 1, n + 2);
						generateWorldMesh();
						updateExposedBlocks();
						updateVAO(complete_mesh);
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "broke block: (" << (int)pos.x << ", " << (int)pos.y << ", " << (int)pos.z << "), " << ms_int << "\n";
}

int World::getBlockType(glm::vec3 position)
{
	glm::vec3 pos = glm::floor(position);

	if (pos.z >= 0 && pos.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (pos.x >= chunks[m][n].chunk_pos.x && pos.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && pos.y >= chunks[m][n].chunk_pos.y && pos.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
				{
					return chunks[m][n].blocks[(int)(pos.x - chunks[m][n].chunk_pos.x)][(int)(pos.y - chunks[m][n].chunk_pos.y)][(int)pos.z].type;
				}
			}
		}
	}
	return 0;
}
