﻿#pragma once

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

	std::vector<float> m_mesh;

	void generateTerrain()
	{
		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int x = 0; x < CHUNK_SIZE.x; x++)
		{
			for (int y = 0; y < CHUNK_SIZE.y; y++)
			{
				double ground_height = perlin.octave2D_01((x + m_chunk_pos.x) * 0.05f, (y + m_chunk_pos.y) * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE.z / 2.0f;

				for (int z = 0; z < CHUNK_SIZE.z; z++)
				{
					if (z < ground_height)
					{
						m_blocks[x][y][z].m_type = 1;
					}
					else
					{
						m_blocks[x][y][z].m_type = 0;
					}
				}
			}
		}
	}

	void generateMesh()
	{
		m_mesh = {};

		for (int x = 0; x < CHUNK_SIZE.x; x++)
		{
			for (int y = 0; y < CHUNK_SIZE.y; y++)
			{
				for (int z = 0; z < CHUNK_SIZE.z; z++)
				{
					m_blocks[x][y][z].m_exposed = false;

					if (m_blocks[x][y][z].m_type > 0)
					{
						float a = m_chunk_pos.x;
						float b = m_chunk_pos.y;

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

						if (x == 0 || m_blocks[x - 1][y][z].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[0][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (x == CHUNK_SIZE.x - 1 || m_blocks[x + 1][y][z].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[1][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (y == 0 || m_blocks[x][y - 1][z].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[2][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (y == CHUNK_SIZE.y - 1 || m_blocks[x][y + 1][z].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[3][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (z == 0 || m_blocks[x][y][z - 1].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[4][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (z == CHUNK_SIZE.z - 1 || m_blocks[x][y][z + 1].m_type == 0)
						{
							for (int i = 0; i < 48; i++)
							{
								m_mesh.push_back(vertices[5][i]);
							}
							m_blocks[x][y][z].m_exposed = true;
						}

						if (m_blocks[x][y][z].m_exposed)
						{
							collision_blocks.push_back(glm::vec3(m_chunk_pos.x + x, m_chunk_pos.y + y, z));
						}
					}
				}
			}
		}
	}
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
				chunk.m_chunk_pos = glm::vec2((m - (int)WORLD_SIZE.x / 2) * CHUNK_SIZE.x, (n - (int)WORLD_SIZE.y / 2) * CHUNK_SIZE.y);
				chunk.generateTerrain();

				m_chunks[m][n] = chunk;
			}
		}
	}

	void shiftChunks()
	{
		if (current_chunk != last_chunk)
		{
			if (current_chunk.x - last_chunk.x == 1)
			{
				for (int m = 0; m < WORLD_SIZE.x; m++)
				{
					for (int n = 0; n < WORLD_SIZE.y; n++)
					{
						if (m == WORLD_SIZE.x - 1)
						{
							m_chunks[m][n].m_chunk_pos += glm::vec2(CHUNK_SIZE.x, 0);
							m_chunks[m][n].generateTerrain();
						}
						else
						{
							m_chunks[m][n] = m_chunks[m + 1][n];
						}
					}
				}
			}
			else if (current_chunk.y - last_chunk.y == 1)
			{
				for (int m = 0; m < WORLD_SIZE.x; m++)
				{
					for (int n = 0; n < WORLD_SIZE.y; n++)
					{
						if (n == WORLD_SIZE.y - 1)
						{
							m_chunks[m][n].m_chunk_pos += glm::vec2(0, CHUNK_SIZE.y);
							m_chunks[m][n].generateTerrain();
						}
						else
						{
							m_chunks[m][n] = m_chunks[m][n + 1];
						}
					}
				}
			}
			else if (current_chunk.x - last_chunk.x == -1)
			{
				for (int m = WORLD_SIZE.x - 1; m >= 0; m--)
				{
					for (int n = 0; n < WORLD_SIZE.y; n++)
					{
						if (m == 0)
						{
							m_chunks[m][n].m_chunk_pos -= glm::vec2(CHUNK_SIZE.x, 0);
							m_chunks[m][n].generateTerrain();
						}
						else
						{
							m_chunks[m][n] = m_chunks[m - 1][n];
						}
					}
				}
			}
			else if (current_chunk.y - last_chunk.y == -1)
			{
				for (int m = 0; m < WORLD_SIZE.x; m++)
				{
					for (int n = WORLD_SIZE.y - 1; n >= 0; n--)
					{
						if (n == 0)
						{
							m_chunks[m][n].m_chunk_pos -= glm::vec2(0, CHUNK_SIZE.y);
							m_chunks[m][n].generateTerrain();
						}
						else
						{
							m_chunks[m][n] = m_chunks[m][n - 1];
						}
					}
				}
			}

			generateWorldMesh();
			setMesh();
		}
		last_chunk = current_chunk;
	}

	void generateChunkMesh()
	{
		m_mesh = {};

		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				m_chunks[m][n].generateMesh();

				for (int i = 0; i < m_chunks[m][n].m_mesh.size(); i++)
				{
					m_mesh.push_back(m_chunks[m][n].m_mesh[i]);
				}
			}
		}
	}

	void generateWorldMesh()
	{
		m_mesh = {};
		collision_blocks = {};

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
							m_chunks[m][n].m_blocks[x][y][z].m_exposed = false;

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

								if (x != 0 && m_chunks[m][n].m_blocks[x - 1][y][z].m_type == 0 || x == 0 && m != 0 && m_chunks[m - 1][n].m_blocks[CHUNK_SIZE.x - 1][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[0][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (x != CHUNK_SIZE.x - 1 && m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0 || x == CHUNK_SIZE.x - 1 && m != WORLD_SIZE.x - 1 && m_chunks[m + 1][n].m_blocks[0][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[1][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (y != 0 && m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0 || y == 0 && n != 0 && m_chunks[m][n - 1].m_blocks[x][CHUNK_SIZE.y - 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[2][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (y != CHUNK_SIZE.y - 1 && m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0 || y == CHUNK_SIZE.y - 1 && n != WORLD_SIZE.y - 1 && m_chunks[m][n + 1].m_blocks[x][0][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[3][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (z != 0 && m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_mesh.push_back(vertices[4][i]);
									}
									m_chunks[m][n].m_blocks[x][y][z].m_exposed = true;
								}

								if (z != CHUNK_SIZE.z - 1 && m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
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

	void setMesh()
	{
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);

		// vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_mesh.size(), m_mesh.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (position)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (texture)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (normal)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void drawMesh()
	{
		// draw vertices
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_mesh.size() / 3);
	}
};