﻿#pragma once

class Block
{
public:
	int m_type = 0;
};

class Chunk
{
public:
	glm::vec2 m_chunk_pos;

	Block m_blocks[16][16][256];
	std::vector<glm::vec3> m_blocks_pos;
};

class World
{
public:

	std::vector<Chunk> m_chunks;
	std::vector<float> m_mesh;

	void generateChunks()
	{
		std::vector<Chunk> chunks;

		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				Chunk chunk;
				chunk.m_chunk_pos = glm::vec2(x * 16, y * 16);

				chunks.push_back(chunk);
			}
		}

		m_chunks = chunks;
	}

	void generateTerrain()
	{
		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int c = 0; c < m_chunks.size(); c++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int y = 0; y < 16; y++)
				{
					double ground_height = perlin.octave2D_01((x + m_chunks[c].m_chunk_pos.x) * 0.05f, (y + m_chunks[c].m_chunk_pos.y) * 0.05f, 4) * 12.0f + 100.0f;

					for (int z = 0; z < ground_height; z++)
					{
						m_chunks[c].m_blocks[x][y][z].m_type = 1;
						m_chunks[c].m_blocks_pos.push_back(glm::vec3(x, y, z));
					}
				}
			}
		}
	}

	void generateMesh()
	{
		std::vector<float> mesh;

		for (int c = 0; c < m_chunks.size(); c++)
		{
			for (int x = 0; x < 16; x++)
			{
				for (int y = 0; y < 16; y++)
				{
					for (int z = 0; z < 256; z++)
					{
						if (m_chunks[c].m_blocks[x][y][z].m_type > 0)
						{
							float a = m_chunks[c].m_chunk_pos.x;
							float b = m_chunks[c].m_chunk_pos.y;

							float vertices[6][48] = {
								// pos.x, pos.y, pos.z, tex.x, tex.y, norm.x, norm.y, norm.z
								// bottom
								{
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
								1.0f + a + x, 1.0f + b + y, 0.0f + z, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
								1.0f + a + x, 0.0f + b + y, 0.0f + z, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
								1.0f + a + x, 1.0f + b + y, 0.0f + z, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
								0.0f + a + x, 1.0f + b + y, 0.0f + z, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
								},
								// top
								{
								0.0f + a + x, 0.0f + b + y, 1.0f + z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								1.0f + a + x, 0.0f + b + y, 1.0f + z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
								0.0f + a + x, 1.0f + b + y, 1.0f + z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
								0.0f + a + x, 0.0f + b + y, 1.0f + z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								},
								// left
								{
								0.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
								0.0f + a + x, 1.0f + b + y, 0.0f + z, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
								0.0f + a + x, 0.0f + b + y, 1.0f + z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
								0.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
								},
								// right
								{
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
								1.0f + a + x, 1.0f + b + y, 0.0f + z, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 1.0f + z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
								},
								// front
								{
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 0.0f + z, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 1.0f + z, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
								1.0f + a + x, 0.0f + b + y, 1.0f + z, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
								0.0f + a + x, 0.0f + b + y, 1.0f + z, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
								0.0f + a + x, 0.0f + b + y, 0.0f + z, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
								},
								// back
								{
								0.0f + a + x, 1.0f + b + y, 0.0f + z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								1.0f + a + x, 1.0f + b + y, 0.0f + z, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								1.0f + a + x, 1.0f + b + y, 1.0f + z, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								0.0f + a + x, 1.0f + b + y, 0.0f + z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								0.0f + a + x, 1.0f + b + y, 1.0f + z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								},
							};

							if (z - 1 < 0 || m_chunks[c].m_blocks[x][y][z - 1].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[0][i]);
								}
							}

							if (z + 1 > 256 || m_chunks[c].m_blocks[x][y][z + 1].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[1][i]);
								}
							}

							if (x - 1 < 0 || m_chunks[c].m_blocks[x - 1][y][z].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[2][i]);
								}
							}

							if (x + 1 > 15 || m_chunks[c].m_blocks[x + 1][y][z].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[3][i]);
								}
							}

							if (y - 1 < 0 || m_chunks[c].m_blocks[x][y - 1][z].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[4][i]);
								}
							}

							if (y + 1 > 15 || m_chunks[c].m_blocks[x][y + 1][z].m_type == 0)
							{
								for (int i = 0; i < 48; i++)
								{
									mesh.push_back(vertices[5][i]);
								}
							}
						}
					}
				}
			}
		}
		m_mesh = mesh;
	}
};