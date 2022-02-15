#pragma once

class Block
{
public:
	int m_type = 0;
	bool m_visible = false;
};

class Chunk
{
public:
	Block m_blocks[CHUNK_SIZE[0]][CHUNK_SIZE[1]][CHUNK_SIZE[2]];

	glm::vec2 m_chunk_pos;
	std::vector<glm::vec3> m_blocks_pos;
};

class World
{
public:
	Chunk m_chunks[WORLD_SIZE[0]][WORLD_SIZE[1]];

	std::vector<float> m_mesh;

	void generateChunks()
	{
		for (int m = 0; m < WORLD_SIZE[0]; m++)
		{
			for (int n = 0; n < WORLD_SIZE[1]; n++)
			{
				Chunk chunk;
				chunk.m_chunk_pos = glm::vec2((m - 4) * CHUNK_SIZE[0], (n - 4) * CHUNK_SIZE[1]);

				m_chunks[m][n] = chunk;
			}
		}
	}

	void generateTerrain()
	{
		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int m = 0; m < WORLD_SIZE[0]; m++)
		{
			for (int n = 0; n < WORLD_SIZE[1]; n++)
			{
				for (int x = 0; x < CHUNK_SIZE[0]; x++)
				{
					for (int y = 0; y < CHUNK_SIZE[1]; y++)
					{
						double ground_height = perlin.octave2D_01((x + m_chunks[m][n].m_chunk_pos.x) * 0.05f, (y + m_chunks[m][n].m_chunk_pos.y) * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE[2] / 2.0f;

						for (int z = 0; z < ground_height; z++)
						{
							m_chunks[m][n].m_blocks[x][y][z].m_type = 1;
							m_chunks[m][n].m_blocks_pos.push_back(glm::vec3(x, y, z));
						}
					}
				}
			}
		}
	}

	void generateMesh()
	{
		for (int m = 0; m < WORLD_SIZE[0]; m++)
		{
			for (int n = 0; n < WORLD_SIZE[1]; n++)
			{
				for (int x = 0; x < CHUNK_SIZE[0]; x++)
				{
					for (int y = 0; y < CHUNK_SIZE[1]; y++)
					{
						for (int z = 0; z < CHUNK_SIZE[2]; z++)
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
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[0][i]);
									}
								}

								if (x + 1 > CHUNK_SIZE[0] - 1 || m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[1][i]);
									}
								}

								if (y - 1 < 0 || m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[2][i]);
									}
								}

								if (y + 1 > CHUNK_SIZE[1] - 1 || m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[3][i]);
									}
								}

								if (z - 1 < 0 || m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[4][i]);
									}
								}

								if (z + 1 > CHUNK_SIZE[2] - 1 || m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_blocks[x][y][z].m_visible = true;
										m_mesh.push_back(vertices[5][i]);
									}
								}
							}
						}
					}
				}
			}
		}
	}
};