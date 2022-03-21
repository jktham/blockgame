#pragma once

#include "global.h"

#include <glm/glm.hpp>

#include <vector>

class Block
{
public:
	int type = 0;
	int data = 0;
};

class Chunk
{
public:
	Block blocks[CHUNK_SIZE.x][CHUNK_SIZE.y][CHUNK_SIZE.z];

	glm::vec2 chunk_pos = glm::vec2(0.0f, 0.0f);

	std::vector<float> mesh;
	std::vector<glm::vec3> exposed_blocks;
	int min_z = CHUNK_SIZE.z - 1;

	void loadChunk();
	void saveChunk();
	void generateChunk();

	// generate terrain for a chunk
	void generateTerrain();
};

class Change
{
public:
	glm::vec3 pos = glm::vec3(0.0f);
	int type = 0;

	Change(glm::vec3 a_pos, int a_type)
	{
		pos = a_pos;
		type = a_type;
	}
};

class World
{
public:
	Chunk chunks[WORLD_SIZE.x][WORLD_SIZE.y];

	std::vector<float> mesh;

	std::vector<glm::vec3> exposed_blocks;
	std::vector<Change> changes;

	// set up initial grid of chunks
	void createChunks();

	// check for chunk change, shift chunks, generate mesh and update VAO
	void updateChunks();
	// shift chunks by given direction and generate new chunk terrain and chunk meshes accordingly
	void shiftChunks(glm::vec2 shift_direction);

	// generate meshes for given range of chunks, cull all hidden faces (including chunk borders) and keep track of exposed blocks per chunk
	void generateChunkMesh(int m_start = 0, int m_end = WORLD_SIZE.x, int n_start = 0, int n_end = WORLD_SIZE.y);
	// combine chunk meshes into world mesh
	void generateWorldMesh();

	// Update VAO and VBO
	void updateVAO();

	// place block of global current type at given position with offset
	void placeBlock(glm::vec3 position, int type);
	// replace block at given position with air
	void destroyBlock(glm::vec3 position);
	// get block type at given position
	int getBlockType(glm::vec3 position);
};