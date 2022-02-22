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

	glm::vec2 m_chunk_pos = glm::vec2(0.0f, 0.0f);

	std::vector<float> m_mesh;
	std::vector<glm::vec3> m_exposed_blocks;
	int m_min_z = CHUNK_SIZE.z - 1;

	// generate terrain for a chunk
	void generateTerrain()
	{
		const siv::PerlinNoise::seed_type seed = TERRAIN_SEED;
		const siv::PerlinNoise perlin{ seed };

		for (int x = 0; x < CHUNK_SIZE.x; x++)
		{
			for (int y = 0; y < CHUNK_SIZE.y; y++)
			{
				double ground_height = perlin.octave2D_01((x + m_chunk_pos.x) * 0.05f, (y + m_chunk_pos.y) * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE.z / 2.0f;

				for (int z = 0; z < CHUNK_SIZE.z; z++)
				{
					if (z == 0)
					{
						m_blocks[x][y][z].m_type = 4;
					}
					else if (z < ground_height - 5)
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

	// check for chunk change, shift chunks, generate mesh and update VAO
	void updateChunks()
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

	// shift chunks by given direction and generate new chunk terrain and chunk meshes accordingly
	void shiftChunks(glm::vec2 shift_direction)
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
						m_chunks[m][n].m_chunk_pos += glm::vec2(CHUNK_SIZE.x, 0);
						m_chunks[m][n].generateTerrain();
					}
					else
					{
						m_chunks[m][n] = m_chunks[m + 1][n];
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
						m_chunks[m][n].m_chunk_pos += glm::vec2(0, CHUNK_SIZE.y);
						m_chunks[m][n].generateTerrain();
					}
					else
					{
						m_chunks[m][n] = m_chunks[m][n + 1];
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
						m_chunks[m][n].m_chunk_pos -= glm::vec2(CHUNK_SIZE.x, 0);
						m_chunks[m][n].generateTerrain();
					}
					else
					{
						m_chunks[m][n] = m_chunks[m - 1][n];
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
						m_chunks[m][n].m_chunk_pos -= glm::vec2(0, CHUNK_SIZE.y);
						m_chunks[m][n].generateTerrain();
					}
					else
					{
						m_chunks[m][n] = m_chunks[m][n - 1];
					}
				}
			}
			generateChunkMesh(0, WORLD_SIZE.x, 0, 2);
		}

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "shifted chunks: (" << shift_direction.x << ", " << shift_direction.y << "), " << ms_int << "\n";
	}

	// generate meshes for given range of chunks, cull all hidden faces (including chunk borders) and keep track of exposed blocks per chunk
	void generateChunkMesh(int m_start = 0, int m_end = WORLD_SIZE.x, int n_start = 0, int n_end = WORLD_SIZE.y)
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		for (int m = m_start; m < m_end; m++)
		{
			for (int n = n_start; n < n_end; n++)
			{
				m_chunks[m][n].m_mesh = {};
				m_chunks[m][n].m_exposed_blocks = {};

				int min_z = m_chunks[m][n].m_min_z;

				if (m != 0 && m_chunks[m - 1][n].m_min_z < m_chunks[m][n].m_min_z && m_chunks[m - 1][n].m_min_z < min_z)
				{
					min_z = m_chunks[m - 1][n].m_min_z;
				}
				if (m != WORLD_SIZE.x - 1 && m_chunks[m + 1][n].m_min_z < m_chunks[m][n].m_min_z && m_chunks[m + 1][n].m_min_z < min_z)
				{
					min_z = m_chunks[m + 1][n].m_min_z;
				}
				if (n != 0 && m_chunks[m][n - 1].m_min_z < m_chunks[m][n].m_min_z && m_chunks[m][n - 1].m_min_z < min_z)
				{
					min_z = m_chunks[m][n - 1].m_min_z;
				}
				if (n != WORLD_SIZE.y && m_chunks[m][n + 1].m_min_z < m_chunks[m][n].m_min_z && m_chunks[m][n + 1].m_min_z < min_z)
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
								float chunk_x = m_chunks[m][n].m_chunk_pos.x;
								float chunk_y = m_chunks[m][n].m_chunk_pos.y;
								float atlas_y = ATLAS_SIZE_Y - m_chunks[m][n].m_blocks[x][y][z].m_type;
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

								if (x != 0 && m_chunks[m][n].m_blocks[x - 1][y][z].m_type == 0 || x == 0 && m != 0 && m_chunks[m - 1][n].m_blocks[CHUNK_SIZE.x - 1][y][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[0][0], &vertices[0][66]);
									exposed = true;
								}

								if (x != CHUNK_SIZE.x - 1 && m_chunks[m][n].m_blocks[x + 1][y][z].m_type == 0 || x == CHUNK_SIZE.x - 1 && m != CHUNK_SIZE.x - 1 && m_chunks[m + 1][n].m_blocks[0][y][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[1][0], &vertices[1][66]);
									exposed = true;
								}

								if (y != 0 && m_chunks[m][n].m_blocks[x][y - 1][z].m_type == 0 || y == 0 && n != 0 && m_chunks[m][n - 1].m_blocks[x][CHUNK_SIZE.y - 1][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[2][0], &vertices[2][66]);
									exposed = true;
								}

								if (y != CHUNK_SIZE.y - 1 && m_chunks[m][n].m_blocks[x][y + 1][z].m_type == 0 || y == CHUNK_SIZE.y - 1 && n != CHUNK_SIZE.y - 1 && m_chunks[m][n + 1].m_blocks[x][0][z].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[3][0], &vertices[3][66]);
									exposed = true;
								}

								if (z != 0 && m_chunks[m][n].m_blocks[x][y][z - 1].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[4][0], &vertices[4][66]);
									exposed = true;
								}

								if (z == CHUNK_SIZE.z - 1 || m_chunks[m][n].m_blocks[x][y][z + 1].m_type == 0)
								{
									m_chunks[m][n].m_mesh.insert(m_chunks[m][n].m_mesh.end(), &vertices[5][0], &vertices[5][66]);
									exposed = true;
								}

								if (exposed)
								{
									m_chunks[m][n].m_exposed_blocks.push_back(glm::ivec3(m_chunks[m][n].m_chunk_pos.x + x, m_chunks[m][n].m_chunk_pos.y + y, z));
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
					exposed_blocks.insert(exposed_blocks.end(), m_chunks[m][n].m_exposed_blocks.begin(), m_chunks[m][n].m_exposed_blocks.end());
				}
			}
		}

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "generated chunk mesh: " << (m_end - m_start) * (n_end - n_start) << " chunks, " << ms_int << "\n";
	}

	// combine chunk meshes into world mesh
	void generateWorldMesh()
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

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "generated world mesh: " << WORLD_SIZE.x * WORLD_SIZE.y << " chunks, " << ms_int << "\n";
	}

	// Update VAO and VBO
	void updateVAO()
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		// vertex array object
		glBindVertexArray(world_VAO);

		// vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, world_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_mesh.size(), m_mesh.data(), GL_STATIC_DRAW);
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
		std::cout << "updated VAO: " << m_mesh.size() << " verts, " << ms_int << "\n";
	}

	// place block of global current type at given position with offset
	void placeBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect)
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
					if (m_chunks[m][n].m_blocks[(int)(position.x - m_chunks[m][n].m_chunk_pos.x)][(int)(position.y - m_chunks[m][n].m_chunk_pos.y)][(int)position.z].m_type != 4)
					{
						if (position.x >= m_chunks[m][n].m_chunk_pos.x && position.x < m_chunks[m][n].m_chunk_pos.x + CHUNK_SIZE.x && position.y >= m_chunks[m][n].m_chunk_pos.y && position.y < m_chunks[m][n].m_chunk_pos.y + CHUNK_SIZE.y)
						{
							m_chunks[m][n].m_blocks[(int)(position.x - m_chunks[m][n].m_chunk_pos.x)][(int)(position.y - m_chunks[m][n].m_chunk_pos.y)][(int)position.z].m_type = current_type;

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

	// replace block at given position with air
	void destroyBlock(std::tuple<glm::vec3, glm::vec3> ray_intersect)
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		glm::vec3 position = std::get<0>(ray_intersect);

		if (position.z >= 0 && position.z < CHUNK_SIZE.z)
		{
			for (int m = WORLD_SIZE.x / 2 - 1; m < WORLD_SIZE.x / 2 + 2; m++)
			{
				for (int n = WORLD_SIZE.y / 2 - 1; n < WORLD_SIZE.y / 2 + 2; n++)
				{
					if (position.x >= m_chunks[m][n].m_chunk_pos.x && position.x < m_chunks[m][n].m_chunk_pos.x + CHUNK_SIZE.x && position.y >= m_chunks[m][n].m_chunk_pos.y && position.y < m_chunks[m][n].m_chunk_pos.y + CHUNK_SIZE.y)
					{
						if (m_chunks[m][n].m_blocks[(int)(position.x - m_chunks[m][n].m_chunk_pos.x)][(int)(position.y - m_chunks[m][n].m_chunk_pos.y)][(int)position.z].m_type != 4)
						{
							m_chunks[m][n].m_blocks[(int)(position.x - m_chunks[m][n].m_chunk_pos.x)][(int)(position.y - m_chunks[m][n].m_chunk_pos.y)][(int)position.z].m_type = 0;

							if (position.z - 1 < m_chunks[m][n].m_min_z)
							{
								m_chunks[m][n].m_min_z = (int)position.z - 1;
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
};