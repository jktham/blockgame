#pragma once

class Terrain
{
public:
	std::vector<glm::vec3> m_offsets;

	void generateTerrainTest()
	{
		const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
		const glm::vec3 size = glm::vec3(50.0f, 1.0f, 50.0f);
		const glm::vec3 step = glm::vec3(1.0f, 1.0f, 1.0f);

		std::vector<glm::vec3> offsets;

		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				for (int k = 0; k < size.z; k++)
				{
					offsets.push_back(glm::vec3(origin.x + i * step.x, origin.y + j * step.y + int(i / 5.0f) + int(k / 5.0f), origin.z + k * step.z));
				}
			}
		}

		m_offsets = offsets;
	}

	void generateTerrainPlane()
	{
		const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
		const glm::vec3 size = glm::vec3(50.0f, 1.0f, 50.0f);
		const glm::vec3 step = glm::vec3(1.0f, 1.0f, 1.0f);

		std::vector<glm::vec3> offsets;

		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				for (int k = 0; k < size.z; k++)
				{
					offsets.push_back(glm::vec3(origin.x + i * step.x, origin.y + j * step.y, origin.z + k * step.z));
				}
			}
		}

		m_offsets = offsets;
	}

	void generateTerrainPerlin()
	{
		const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
		const glm::vec3 size = glm::vec3(100.0f, 1.0f, 100.0f);

		std::vector<glm::vec3> offsets;

		const siv::PerlinNoise::seed_type seed = 123456u;
		const siv::PerlinNoise perlin{ seed };

		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				for (int k = 0; k < size.z; k++)
				{
					double height = perlin.octave2D_01(i*0.05f, k*0.05f, 4);
					offsets.push_back(glm::vec3(i, int(height * 10.0f)-j, k));
				}
			}
		}

		m_offsets = offsets;
	}

	void setInstances()
	{
		// instance vertex buffer object
		unsigned int instanceVBO;
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (m_offsets.size()), &m_offsets[0], GL_STATIC_DRAW);
		glVertexAttribDivisor(1, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (instance offsets)
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};