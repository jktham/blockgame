#pragma once
#include <chrono>

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
	int m_min_z = CHUNK_SIZE.z - 1;

	// generate terrain for a chunk
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
					if (z < ground_height - 5)
					{
						m_blocks[x][y][z].m_type = 3;
					}
					else if (z < ground_height - 1)
					{
						m_blocks[x][y][z].m_type = 2;
					}
					else if (z < ground_height - 0)
					{
						m_blocks[x][y][z].m_type = 1;
					}
					else
					{
						m_blocks[x][y][z].m_type = 0;
					}

					if (m_blocks[x][y][z].m_type == 0)
					{
						if (z - 1 < m_min_z)
						{
							m_min_z = z - 1;
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

	// set up initial grid of chunks
	void createChunks()
	{
		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				m_chunks[m][n].m_chunk_pos = glm::vec2((m - WORLD_SIZE.x / 2) * CHUNK_SIZE.x, (n - WORLD_SIZE.y / 2) * CHUNK_SIZE.y);
				m_chunks[m][n].generateTerrain();
			}
		}
	}

	// check for chunk change and shift chunks accordingly, while generating a row of new chunks
	void shiftChunks()
	{
		if (current_chunk != last_chunk)
		{
			auto t1 = std::chrono::high_resolution_clock::now();

			glm::ivec2 shift_dir = current_chunk - last_chunk;

			if (shift_dir.x == 1)
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
				generateMesh(WORLD_SIZE.y - 2, WORLD_SIZE.y, 0, WORLD_SIZE.y);
			}
			if (shift_dir.y == 1)
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
				generateMesh(0, WORLD_SIZE.x, WORLD_SIZE.y - 2, WORLD_SIZE.y);
			}
			if (shift_dir.x == -1)
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
				generateMesh(0, 2, 0, WORLD_SIZE.y);
			}
			if (shift_dir.y == -1)
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
				generateMesh(0, WORLD_SIZE.x, 0, 2);
			}

			updateMesh();

			auto t2 = std::chrono::high_resolution_clock::now();
			auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
			std::cout << "shifted chunks: (" << shift_dir.x << ", " << shift_dir.y << "), " << ms_int << "\n";
		}

		last_chunk = current_chunk;
	}

	// generate meshes for given range of chunks, cull all hidden faces (including chunk borders) and keep track of exposed blocks per chunk
	void generateMesh(int m_start = 0, int m_end = WORLD_SIZE.x, int n_start = 0, int n_end = WORLD_SIZE.y)
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		for (int m = m_start; m < m_end; m++)
		{
			for (int n = n_start; n < n_end; n++)
			{
				m_chunks[m][n].m_mesh = {};
				m_chunks[m][n].m_exposed_blocks = {};

				int min_z = m_chunks[m][n].m_min_z;

				if (m != 0 && m_chunks[m - 1][n].m_min_z < m_chunks[m][n].m_min_z)
				{
					min_z = m_chunks[m - 1][n].m_min_z;
				}
				if (m != WORLD_SIZE.x - 1 && m_chunks[m + 1][n].m_min_z < m_chunks[m][n].m_min_z)
				{
					min_z = m_chunks[m + 1][n].m_min_z;
				}
				if (n != 0 && m_chunks[m][n - 1].m_min_z < m_chunks[m][n].m_min_z)
				{
					min_z = m_chunks[m][n - 1].m_min_z;
				}
				if (n != WORLD_SIZE.y && m_chunks[m][n + 1].m_min_z < m_chunks[m][n].m_min_z)
				{
					min_z = m_chunks[m][n + 1].m_min_z;
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

							if (m_chunks[m][n].m_blocks[x][y][z].m_type > 0)
							{
								float a = m_chunks[m][n].m_chunk_pos.x;
								float b = m_chunks[m][n].m_chunk_pos.y;
								int c = 6 - m_chunks[m][n].m_blocks[x][y][z].m_type;
								float d = 1.0f / 6.0f;

								float vertices[6][48] = {
									// pos.x, pos.y, pos.z, tex.x, tex.y, norm.x, norm.y, norm.z
									// left
									{
									0.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 0.0f) * d, (1.0f + c) * d, -1.0f, 0.0f, 0.0f,
									0.0f + a + x, 1.0f + b + y, 0.0f + z, (1.0f + 0.0f) * d, (0.0f + c) * d, -1.0f, 0.0f, 0.0f,
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 0.0f) * d, (0.0f + c) * d, -1.0f, 0.0f, 0.0f,
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 0.0f) * d, (0.0f + c) * d, -1.0f, 0.0f, 0.0f,
									0.0f + a + x, 0.0f + b + y, 1.0f + z, (0.0f + 0.0f) * d, (1.0f + c) * d, -1.0f, 0.0f, 0.0f,
									0.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 0.0f) * d, (1.0f + c) * d, -1.0f, 0.0f, 0.0f,
									},
									// right
									{
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 1.0f) * d, (1.0f + c) * d, 1.0f, 0.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 1.0f) * d, (0.0f + c) * d, 1.0f, 0.0f, 0.0f,
									1.0f + a + x, 1.0f + b + y, 0.0f + z, (1.0f + 1.0f) * d, (0.0f + c) * d, 1.0f, 0.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 1.0f) * d, (0.0f + c) * d, 1.0f, 0.0f, 0.0f,
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 1.0f) * d, (1.0f + c) * d, 1.0f, 0.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 1.0f + z, (0.0f + 1.0f) * d, (1.0f + c) * d, 1.0f, 0.0f, 0.0f,
									},
									// front
									{
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 2.0f) * d, (0.0f + c) * d, 0.0f, -1.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 0.0f + z, (1.0f + 2.0f) * d, (0.0f + c) * d, 0.0f, -1.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 1.0f + z, (1.0f + 2.0f) * d, (1.0f + c) * d, 0.0f, -1.0f, 0.0f,
									1.0f + a + x, 0.0f + b + y, 1.0f + z, (1.0f + 2.0f) * d, (1.0f + c) * d, 0.0f, -1.0f, 0.0f,
									0.0f + a + x, 0.0f + b + y, 1.0f + z, (0.0f + 2.0f) * d, (1.0f + c) * d, 0.0f, -1.0f, 0.0f,
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 2.0f) * d, (0.0f + c) * d, 0.0f, -1.0f, 0.0f,
									},
									// back
									{
									0.0f + a + x, 1.0f + b + y, 0.0f + z, (0.0f + 3.0f) * d, (0.0f + c) * d, 0.0f, 1.0f, 0.0f,
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 3.0f) * d, (1.0f + c) * d, 0.0f, 1.0f, 0.0f,
									1.0f + a + x, 1.0f + b + y, 0.0f + z, (1.0f + 3.0f) * d, (0.0f + c) * d, 0.0f, 1.0f, 0.0f,
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 3.0f) * d, (1.0f + c) * d, 0.0f, 1.0f, 0.0f,
									0.0f + a + x, 1.0f + b + y, 0.0f + z, (0.0f + 3.0f) * d, (0.0f + c) * d, 0.0f, 1.0f, 0.0f,
									0.0f + a + x, 1.0f + b + y, 1.0f + z, (0.0f + 3.0f) * d, (1.0f + c) * d, 0.0f, 1.0f, 0.0f,
									},
									// bottom
									{
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 4.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, -1.0f,
									1.0f + a + x, 1.0f + b + y, 0.0f + z, (1.0f + 4.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, -1.0f,
									1.0f + a + x, 0.0f + b + y, 0.0f + z, (1.0f + 4.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, -1.0f,
									1.0f + a + x, 1.0f + b + y, 0.0f + z, (1.0f + 4.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, -1.0f,
									0.0f + a + x, 0.0f + b + y, 0.0f + z, (0.0f + 4.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, -1.0f,
									0.0f + a + x, 1.0f + b + y, 0.0f + z, (0.0f + 4.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, -1.0f,
									},
									// top
									{
									0.0f + a + x, 0.0f + b + y, 1.0f + z, (0.0f + 5.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, 1.0f,
									1.0f + a + x, 0.0f + b + y, 1.0f + z, (1.0f + 5.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, 1.0f,
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 5.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, 1.0f,
									1.0f + a + x, 1.0f + b + y, 1.0f + z, (1.0f + 5.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, 1.0f,
									0.0f + a + x, 1.0f + b + y, 1.0f + z, (0.0f + 5.0f) * d, (1.0f + c) * d, 0.0f, 0.0f, 1.0f,
									0.0f + a + x, 0.0f + b + y, 1.0f + z, (0.0f + 5.0f) * d, (0.0f + c) * d, 0.0f, 0.0f, 1.0f,
									},
								};

								if (x != 0 && m_chunks[m][n].m_blocks[x - 1][y][z].m_type == 0 || x == 0 && m != 0 && m_chunks[m - 1][n].m_blocks[CHUNK_SIZE.x - 1][y][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[0][0], &vertices[0][48]);
									exposed = true;
								}

								if (x != CHUNK_SIZE.x - 1 && m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0 || x == CHUNK_SIZE.x - 1 && m != CHUNK_SIZE.x - 1 && m_chunks[m + 1][n].m_blocks[0][y][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[1][0], &vertices[1][48]);
									exposed = true;
								}

								if (y != 0 && m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0 || y == 0 && n != 0 && m_chunks[m][n - 1].m_blocks[x][CHUNK_SIZE.y - 1][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[2][0], &vertices[2][48]);
									exposed = true;
								}

								if (y != CHUNK_SIZE.y - 1 && m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0 || y == CHUNK_SIZE.y - 1 && n != CHUNK_SIZE.y - 1 && m_chunks[m][n + 1].m_blocks[x][0][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[3][0], &vertices[3][48]);
									exposed = true;
								}

								if (z != 0 && m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[4][0], &vertices[4][48]);
									exposed = true;
								}

								if (z != CHUNK_SIZE.z - 1 && m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[5][0], &vertices[5][48]);
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

		exposed_blocks = {};

		for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
		{
			for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
			{
				exposed_blocks.insert(exposed_blocks.end(), m_chunks[m][n].m_exposed_blocks.begin(), m_chunks[m][n].m_exposed_blocks.end());
			}
		}

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "generated mesh: " << (m_end - m_start) * (n_end - n_start) << " chunks, " << ms_int << "\n";
	}

	// stitch together chunk meshes and update VAO and VBO
	void updateMesh()
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		m_mesh = {};

		for (int m = 0; m < WORLD_SIZE.x; m++)
		{
			for (int n = 0; n < WORLD_SIZE.y; n++)
			{
				m_mesh.insert(m_mesh.end(), m_chunks[m][n].m_mesh.begin(), m_chunks[m][n].m_mesh.end());
			}
		}

		// vertex array object
		glBindVertexArray(VAO);

		// vertex buffer object
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

		glBindVertexArray(0);

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "updated mesh: " << m_mesh.size() << " verts, " << ms_int << "\n";
	}
};