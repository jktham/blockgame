#pragma once

class Block
{
public:
	int m_type = 0;
	bool m_exposed = false;
};

class Chunk
{
public:
	Block m_blocks[CHUNK_SIZE.x][CHUNK_SIZE.y][CHUNK_SIZE.z];

	glm::vec2 m_chunk_pos;
};

class World
{
public:
	Chunk m_chunks[WORLD_SIZE.x][WORLD_SIZE.y];

	std::vector<float> m_mesh;

	void generateChunks()
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				Chunk chunk;
				chunk.m_chunk_pos = glm::vec2((m - 4) * CHUNK_SIZE.x, (n - 4) * CHUNK_SIZE.y);

				m_chunks[m][n] = chunk;
			}
		}
	}

	void generateTerrain()
	{
		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				for (int x = 0; x < CHUNK_SIZE.x; x++)
				{
					for (int y = 0; y < CHUNK_SIZE.y; y++)
					{
						double ground_height = perlin.octave2D_01((x + m_chunks[m][n].m_chunk_pos.x) * 0.05f, (y + m_chunks[m][n].m_chunk_pos.y) * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE.z / 2.0f;

						for (int z = 0; z < ground_height; z++)
						{
							m_chunks[m][n].m_blocks[x][y][z].m_type = 1;
						}
					}
				}
			}
		}
	}

	void generateMesh()
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				for (int x = 0; x < CHUNK_SIZE.x; x++)
				{
					for (int y = 0; y < CHUNK_SIZE.y; y++)
					{
						for (int z = 0; z < CHUNK_SIZE.z; z++)
						{
							if (m_chunks[m][n].m_blocks[x][y][z].m_type > 0)
							{
								float a = m_chunks[m][n].m_chunk_pos.x;
								float b = m_chunks[m][n].m_chunk_pos.y;

								float vertices[6][48] = {
									// pos.x, pos.y, pos.z, tex.x, tex.y, norm.x, norm.y, norm.z
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
								};

								if (x - 1 < 0 || m_chunks[m][n].m_blocks[x - 1][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[0][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (x + 1 > CHUNK_SIZE.x - 1 || m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[1][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (y - 1 < 0 || m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[2][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (y + 1 > CHUNK_SIZE.y - 1 || m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[3][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (z - 1 < 0 || m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[4][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (z + 1 > CHUNK_SIZE.z - 1 || m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[5][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (m_chunks[m][n].m_blocks[x][y][z].m_exposed)
								{
									collision_blocks.push_back(glm::vec3(m_chunks[m][n].m_chunk_pos.x + x, m_chunks[m][n].m_chunk_pos.y + y, z));
								}
							}
						}
					}
				}
			}
		}
	}
};