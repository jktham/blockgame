#pragma once

class UI
{
public:
	std::vector<float> m_mesh;

	void update()
	{
		generateMesh();
		updateVAO();
	}

	void generateMesh()
	{
		m_mesh = {};

		std::vector<float> crosshair = {
			// pos.x, pos.y, color.r, color.g, color.b, tex.x, tex.y
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		};

		float atlas_y = ATLAS_SIZE_Y - current_type;

		std::vector<float> block_icon = {
			// pos.x, pos.y, color.r, color.g, color.b, tex.x, tex.y -> use texture if color is -1
			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 1.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y,

			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y,
			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 0.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, -1.0f, -1.0f, -1.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y,
		};

		m_mesh.insert(m_mesh.end(), crosshair.begin(), crosshair.end());
		m_mesh.insert(m_mesh.end(), block_icon.begin(), block_icon.end());
	}

	void updateVAO()
	{
		// vertex array object
		glBindVertexArray(ui_VAO);

		// vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_mesh.size(), m_mesh.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (position)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (color)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (texture)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
};