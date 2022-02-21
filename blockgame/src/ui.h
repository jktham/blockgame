#pragma once

class UI
{
public:
	std::vector<float> m_mesh;

	void generateMesh()
	{
		m_mesh = {};

		std::vector<float> crosshair = {
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH,
		};

		m_mesh.insert(m_mesh.end(), crosshair.begin(), crosshair.end());
	}

	void updateMesh()
	{
		// vertex array object
		glBindVertexArray(ui_VAO);

		// vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_mesh.size(), m_mesh.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (position)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
};