#pragma once

class Block
{
public:
	int m_type = 0;
};

class Chunk
{
public:
	Block m_blocks[CHUNK_SIZE.x][CHUNK_SIZE.y][CHUNK_SIZE.z];

	glm::vec2 m_chunk_pos;

	std::vector<float> m_mesh;
	std::vector<glm::vec3> m_exposed_blocks;

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
				chunk.m_chunk_pos = glm::vec2((m - WORLD_SIZE.x / 2) * CHUNK_SIZE.x, (n - WORLD_SIZE.y / 2) * CHUNK_SIZE.y);
				chunk.generateTerrain();

				m_chunks[m][n] = chunk;
			}
		}
	}

	void shiftChunks()
	{
		if (current_chunk != last_chunk)
		{
			std::cout << "chunk changed\n";

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
				generateWorldMesh(WORLD_SIZE.y - 2, WORLD_SIZE.y, 0, WORLD_SIZE.y);
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
				generateWorldMesh(0, WORLD_SIZE.x, WORLD_SIZE.y-2, WORLD_SIZE.y);
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
				generateWorldMesh(0, 2, 0, WORLD_SIZE.y);
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
				generateWorldMesh(0, WORLD_SIZE.x, 0, 2);
			}

			setMesh();
		}
		last_chunk = current_chunk;
	}

	void generateWorldMesh(int m_start = 0, int m_end = WORLD_SIZE.x, int n_start = 0, int n_end = WORLD_SIZE.y)
	{
		for (int m = m_start; m < m_end; m++)
		{
			for (int n = n_start; n < n_end; n++)
			{
				m_chunks[m][n].m_mesh = {};
				m_chunks[m][n].m_exposed_blocks = {};

				for (int x = 0; x < CHUNK_SIZE.x; x++)
				{
					for (int y = 0; y < CHUNK_SIZE.y; y++)
					{
						for (int z = 0; z < CHUNK_SIZE.z; z++)
						{
							bool exposed = false;

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
										m_chunks[m][n].m_mesh.push_back(vertices[0][i]);
									}
									exposed = true;
								}

								if (x != CHUNK_SIZE.x - 1 && m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0 || x == CHUNK_SIZE.x - 1 && m != CHUNK_SIZE.x - 1 && m_chunks[m + 1][n].m_blocks[0][y][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_mesh.push_back(vertices[1][i]);
									}
									exposed = true;
								}

								if (y != 0 && m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0 || y == 0 && n != 0 && m_chunks[m][n - 1].m_blocks[x][CHUNK_SIZE.y - 1][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_mesh.push_back(vertices[2][i]);
									}
									exposed = true;
								}

								if (y != CHUNK_SIZE.y - 1 && m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0 || y == CHUNK_SIZE.y - 1 && n != CHUNK_SIZE.y - 1 && m_chunks[m][n + 1].m_blocks[x][0][z].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_mesh.push_back(vertices[3][i]);
									}
									exposed = true;
								}

								if (z != 0 && m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_mesh.push_back(vertices[4][i]);
									}
									exposed = true;
								}

								if (z != CHUNK_SIZE.z - 1 && m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
								{
									for (int i = 0; i < 48; i++)
									{
										m_chunks[m][n].m_mesh.push_back(vertices[5][i]);
									}
									exposed = true;
								}

								if (exposed)
								{
									m_chunks[m][n].m_exposed_blocks.push_back(glm::vec3(m_chunks[m][n].m_chunk_pos.x + x, m_chunks[m][n].m_chunk_pos.y + y, z));
								}
							}
						}
					}
				}
			}
		}

		collision_blocks = {};

		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				collision_blocks.insert(collision_blocks.end(), m_chunks[m][n].m_exposed_blocks.begin(), m_chunks[m][n].m_exposed_blocks.end());
			}
		}
	}

	void setMesh()
	{
		std::cout << "updating mesh\n";

		m_mesh = {};

		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				m_mesh.insert(m_mesh.end(), m_chunks[m][n].m_mesh.begin(), m_chunks[m][n].m_mesh.end());
			}
		}

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