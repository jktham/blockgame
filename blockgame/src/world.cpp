﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>

#include "global.h"
#include "terrain.h"
#include "world.h"

void Chunk::loadChunk()
{
	generateTerrain();
}

void Chunk::saveChunk()
{
	/*std::string output;

	output.append(std::to_string(chunk_pos.x));
	output.append(",");
	output.append(std::to_string(chunk_pos.y));
	output.append("\n");

	for (int i = 0; i < CHUNK_SIZE.x; i++)
	{
		for (int j = 0; j < CHUNK_SIZE.y; j++)
		{
			for (int k = 0; k < CHUNK_SIZE.z; k++)
			{
				output.append(std::to_string(blocks[i][j][k].type));
				output.append(",");
			}
		}
	}
	output.append("\n");

	std::ofstream file("dat/chunks.txt", std::ios_base::app | std::ios_base::out);
	file << output;
	file.close();*/
}

void Chunk::generateChunk()
{
	if (std::find(generated_chunks.begin(), generated_chunks.end(), chunk_pos) != generated_chunks.end()) {
		loadChunk();
	}
	else {
		generateTerrain();
	}
}

void Chunk::generateTerrain()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	for (int x = 0; x < CHUNK_SIZE.x; x++)
	{
		for (int y = 0; y < CHUNK_SIZE.y; y++)
		{
			double ground_height = terrain->getGroundHeight(x + (int)chunk_pos.x, y + (int)chunk_pos.y);

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
				else if (z < ground_height - 0)
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
	generated_chunks.push_back(chunk_pos);
	saveChunk();

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "generated terrain: (" << chunk_pos.x << ", " << chunk_pos.y << "), " << ms_int << "\n";
}

void World::createChunks()
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

void World::updateChunks()
{
	if (current_chunk != last_chunk)
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		glm::vec2 shift_direction = current_chunk - last_chunk;

		shiftChunks(shift_direction);
		generateWorldMesh();
		updateVAO();

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "updated chunks: total " << ms_int << "\n";
	}

	last_chunk = current_chunk;
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

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "generated world mesh: " << WORLD_SIZE.x * WORLD_SIZE.y << " chunks, " << ms_int << "\n";
}

void World::updateVAO()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	// vertex array object
	glBindVertexArray(world_VAO);

	// vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(), GL_STATIC_DRAW);
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
	std::cout << "updated VAO: " << mesh.size() << " verts, " << ms_int << "\n";
}

void World::placeBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	glm::vec3 position = std::get<0>(ray_intersect);
	glm::vec3 offset = std::get<1>(ray_intersect);

	if (abs(offset.x) > abs(offset.y) && abs(offset.x) > abs(offset.z))
	{
		position.x += (int)glm::sign(offset.x);
	}
	if (abs(offset.y) > abs(offset.z) && abs(offset.y) > abs(offset.x))
	{
		position.y += (int)glm::sign(offset.y);
	}
	if (abs(offset.z) > abs(offset.x) && abs(offset.z) > abs(offset.y))
	{
		position.z += (int)glm::sign(offset.z);
	}

	if (position.z >= 0 && position.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type != 4)
				{
					if (position.x >= chunks[m][n].chunk_pos.x && position.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && position.y >= chunks[m][n].chunk_pos.y && position.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
					{
						chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type = current_type;

						generateChunkMesh(m - 1, m + 2, n - 1, n + 2);
						generateWorldMesh();
						updateVAO();
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "placed block: (" << (int)position.x << ", " << (int)position.y << ", " << (int)position.z << "), " << current_type << ", total " << ms_int << "\n";
}

void World::destroyBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect)
{
	auto t1 = std::chrono::high_resolution_clock::now();

	glm::vec3 position = std::get<0>(ray_intersect);

	if (position.z >= 0 && position.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (position.x >= chunks[m][n].chunk_pos.x && position.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && position.y >= chunks[m][n].chunk_pos.y && position.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
				{
					if (chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type != 4)
					{
						chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type = 0;

						if (position.z - 1 < chunks[m][n].min_z)
						{
							chunks[m][n].min_z = (int)position.z - 1;
						}

						generateChunkMesh(m - 1, m + 2, n - 1, n + 2);
						generateWorldMesh();
						updateVAO();
					}
				}
			}
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "destroyed block: (" << (int)position.x << ", " << (int)position.y << ", " << (int)position.z << "), total " << ms_int << "\n";
}

int World::getBlockType(std::tuple<glm::vec3, glm::vec3> ray_intersect)
{
	glm::vec3 position = std::get<0>(ray_intersect);

	if (position.z >= 0 && position.z < CHUNK_SIZE.z)
	{
		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				if (position.x >= chunks[m][n].chunk_pos.x && position.x < chunks[m][n].chunk_pos.x + CHUNK_SIZE.x && position.y >= chunks[m][n].chunk_pos.y && position.y < chunks[m][n].chunk_pos.y + CHUNK_SIZE.y)
				{
					if (chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type != 0)
					{
						return chunks[m][n].blocks[(int)(position.x - chunks[m][n].chunk_pos.x)][(int)(position.y - chunks[m][n].chunk_pos.y)][(int)position.z].type;
					}
				}
			}
		}
	}
	return current_type;
}