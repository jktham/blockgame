#include "terrain.h"
#include "world.h"
#include "global.h"

#include <glm/glm.hpp>

#include <vector>
#include <iostream>
#include <random>

void Terrain::generateChunkTerrain(Chunk* chunk)
{
	// generate ground
	for (int x = 0; x < CHUNK_SIZE.x; x++)
	{
		for (int y = 0; y < CHUNK_SIZE.y; y++)
		{
			float ground_height = getGroundHeight(x + (int)chunk->chunk_pos.x, y + (int)chunk->chunk_pos.y);

			for (int z = 0; z < CHUNK_SIZE.z; z++)
			{
				if (z == 0)
				{
					chunk->blocks[x][y][z].type = 4;
				}
				else if (z < (int)ground_height - 5)
				{
					chunk->blocks[x][y][z].type = 3;
				}
				else if (z < (int)ground_height - 1)
				{
					chunk->blocks[x][y][z].type = 2;
				}
				else if (z < (int)ground_height)
				{
					chunk->blocks[x][y][z].type = 1;
				}
				else
				{
					chunk->blocks[x][y][z].type = 0;
				}

				if (chunk->blocks[x][y][z].type == 0)
				{
					if (z - 1 < chunk->min_z)
					{
						chunk->min_z = z - 1;
					}
				}
			}
		}
	}

	// generate water
	float water_level = 21.0f;
	for (int x = 0; x < CHUNK_SIZE.x; x++)
	{
		for (int y = 0; y < CHUNK_SIZE.y; y++)
		{
			for (int z = chunk->min_z; z < water_level; z++)
			{
				if (chunk->blocks[x][y][z].type == 0)
					chunk->blocks[x][y][z].type = 8;
			}
		}
	}

	int terrain_params = (int)(OFFSET + AMPLITUDE + FREQUENCY + OCTAVES + SHIFT + LACUNARITY + PERSISTENCE);

	int a = (int)chunk->chunk_pos.x;
	int b = (int)chunk->chunk_pos.y;
	a = (a < 0) ? (a * -2) - 1 : (a * 2);
	b = (b < 0) ? (b * -2) - 1 : (b * 2);
	int cantor = (int)(0.5f * (a + b) * (a + b + 1) + b);

	unsigned int seed = terrain_params + cantor;

	std::mt19937 rnd(seed);
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// generate trees
	for (int x = 2; x < CHUNK_SIZE.x - 2; x++)
	{
		for (int y = 2; y < CHUNK_SIZE.y - 2; y++)
		{
			float ground_height = terrain->getGroundHeight(x + (int)chunk->chunk_pos.x, y + (int)chunk->chunk_pos.y);

			if (dis(rnd) < 0.01f && ground_height < CHUNK_SIZE.z - 8.0f && chunk->blocks[x][y][(int)ground_height].type == 0)
			{
				int tree_height = (int)(5.0f + dis(rnd) * 3.0f);

				bool tree_adjacent = false;
				for (int ix = -1; ix <= 1; ix++)
					for (int iy = -1; iy <= 1; iy++)
						for (int i = 0; i < tree_height; i++)
							if (chunk->blocks[x + ix][y + iy][(int)ground_height + i].type == 6)
								tree_adjacent = true;

				if (tree_adjacent)
					continue;

				// trunk
				for (int i = 0; i < tree_height; i++)
					chunk->blocks[x][y][(int)ground_height + i].type = 6;

				// layer 1
				for (int ix = -1; ix <= 1; ix++)
					for (int iy = -1; iy <= 1; iy++)
						if (chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height].type == 0)
						{
							if (abs(ix * iy) < 1)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height].type = 7;
						}

				// layer 2
				for (int ix = -1; ix <= 1; ix++)
					for (int iy = -1; iy <= 1; iy++)
						if (chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 1].type == 0)
						{
							if (abs(ix * iy) < 1)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 1].type = 7;
							else if (dis(rnd) < 0.5f)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 1].type = 7;
						}

				// layer 3
				for (int ix = -2; ix <= 2; ix++)
					for (int iy = -2; iy <= 2; iy++)
						if ((chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 2].type == 0))
						{
							if (abs(ix * iy) < 4)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 2].type = 7;
							else if (dis(rnd) < 0.4f)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 2].type = 7;
						}

				// layer 4
				for (int ix = -2; ix <= 2; ix++)
					for (int iy = -2; iy <= 2; iy++)
						if ((chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 3].type == 0))
						{
							if (abs(ix * iy) < 4)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 3].type = 7;
							else if (dis(rnd) < 0.5f)
								chunk->blocks[x + ix][y + iy][(int)ground_height + tree_height - 3].type = 7;
						}
			}
		}
	}

	// generate ores
	for (int x = 0; x < CHUNK_SIZE.x; x++)
	{
		for (int y = 0; y < CHUNK_SIZE.y; y++)
		{
			float ground_height = getGroundHeight(x + (int)chunk->chunk_pos.x, y + (int)chunk->chunk_pos.y);
			for (int z = 1; z < ground_height; z++)
			{
				float depth = abs(ground_height - z);
				if (dis(rnd) < 0.001f * (depth - 8.0f) && chunk->blocks[x][y][z].type == 3)
				{
					chunk->blocks[x][y][z].type = 11;
				}
				if (dis(rnd) < 0.0001f * (depth - 8.0f) && chunk->blocks[x][y][z].type == 3)
				{
					chunk->blocks[x][y][z].type = 12;
				}
				if (dis(rnd) < 0.00001f * (depth - 8.0f) && chunk->blocks[x][y][z].type == 3)
				{
					chunk->blocks[x][y][z].type = 13;
				}
			}
		}
	}
}

float Terrain::getGroundHeight(int x, int y)
{
	float h = (float)CHUNK_SIZE.z / 2.0f;

	int offset = OFFSET;
	float frequency = FREQUENCY;
	float amplitude = AMPLITUDE;

	for (int i = 0; i < OCTAVES; i++)
	{
		h += getPerlin(x + offset, y + offset, frequency, amplitude);
		offset += SHIFT;
		frequency *= LACUNARITY;
		amplitude *= PERSISTENCE;
	}

	//h = sin(x * 0.1f) * 10.0f + 32.0f;
	//h = (x * x + y * y) * 0.1f;
	//h = abs(x - y);
	return h;
}

float Terrain::interpolate(float a, float b, float w)
{
	//return (b - a) * w + a;
	//return (b - a) * (float)(3.0f * pow(w, 2) - 2.0f * pow(w, 3)) + a;
	return (b - a) * (float)(6.0f * pow(w, 5) - 15.0f * pow(w, 4) + 10.0f * pow(w, 3)) + a;
}

glm::vec2 Terrain::getRandomGridVector(int x, int y)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;

	unsigned a = x;
	unsigned b = y;

	a *= 3284157443;
	b ^= a << s | a >> (w - s);
	b *= 1911520717;
	a ^= b << s | b >> (w - s);
	a *= 2048419325;
	float r = a * (3.14159265f / ~(~0u >> 1));

	glm::vec2 v = glm::vec2(0.0f);
	v.x = cos(r);
	v.y = sin(r);

	return v;
}

float Terrain::getDotProduct(int x, int y, float fx, float fy)
{
	glm::vec2 g = getRandomGridVector(x, y);

	float dx = fx - (float)x;
	float dy = fy - (float)y;

	return (dx * g.x + dy * g.y);
}

float Terrain::getPerlin(int x, int y, float frequency, float amplitude)
{
	float fx = (float)x / (float)CHUNK_SIZE.x * frequency;
	float fy = (float)y / (float)CHUNK_SIZE.y * frequency;

	int x0 = (int)floor(fx);
	int x1 = x0 + 1;
	int y0 = (int)floor(fy);
	int y1 = y0 + 1;

	float wx = fx - (float)x0;
	float wy = fy - (float)y0;

	float n0, n1;
	float v0, v1;
	float v;

	n0 = getDotProduct(x0, y0, fx, fy);
	n1 = getDotProduct(x1, y0, fx, fy);
	v0 = interpolate(n0, n1, wx);

	n0 = getDotProduct(x0, y1, fx, fy);
	n1 = getDotProduct(x1, y1, fx, fy);
	v1 = interpolate(n0, n1, wx);

	v = interpolate(v0, v1, wy);

	return v * amplitude;
}