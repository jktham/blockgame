#pragma once

#include <vector>
#include <glm/glm.hpp>

class Block
{
public:
	int type = 0;
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

class World
{
public:
	Chunk chunks[WORLD_SIZE.x][WORLD_SIZE.y];

	std::vector<float> mesh;

	std::vector<glm::vec3> exposed_blocks;
	std::vector<glm::vec3> reachable_blocks;
	std::vector<glm::vec2> generated_chunks;

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
	void placeBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect);
	// replace block at given position with air
	void destroyBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect);
	// get block type at given position
	int getBlockType(std::tuple<glm::vec3, glm::vec3> ray_intersect);
};