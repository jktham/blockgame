#pragma once

struct Glyph {
	glm::vec2 m_tex_coord;
	glm::vec2 m_size;
	float m_space;
};

std::map<int, Glyph> Glyphs;

class Label
{
public:
	glm::vec2 m_pos = glm::vec3(0.0f);
	glm::vec3 m_color = glm::vec3(0.0f);
	std::vector<float> m_mesh;
	std::string m_text = "";
	float m_scale = 1.0f;

	void generateMesh()
	{
		m_mesh = {};

		float x = m_pos.x;
		float y = m_pos.y;

		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			Glyph ch = Glyphs[*c];

			float w = 1.0f * m_scale;
			float h = 1.0f * m_scale;

			std::vector<float> character_vertices = {
				x,     y + h, m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x,			   ch.m_tex_coord.y,			   2.0f,
				x,     y,     m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x,			   ch.m_tex_coord.y + ch.m_size.y, 2.0f,
				x + w, y,     m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x + ch.m_size.x, ch.m_tex_coord.y + ch.m_size.y, 2.0f,

				x,     y + h, m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x,			   ch.m_tex_coord.y,			   2.0f,
				x + w, y,     m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x + ch.m_size.x, ch.m_tex_coord.y + ch.m_size.y, 2.0f,
				x + w, y + h, m_color.r, m_color.g, m_color.b, ch.m_tex_coord.x + ch.m_size.x, ch.m_tex_coord.y,			   2.0f,
			};

			x += ch.m_space * m_scale;

			m_mesh.insert(m_mesh.end(), character_vertices.begin(), character_vertices.end());
		}
	}
};

class Button
{
public:
	glm::vec2 m_pos = glm::vec3(0.0f);
	glm::vec2 m_width = glm::vec3(0.0f);
	glm::vec3 m_color = glm::vec3(0.0f);
	bool m_clicked = false;
	std::vector<float> m_mesh;
	std::function<void()> action = []() { std::cout << "click\n"; };

	void generateMesh()
	{
		m_mesh = {};

		std::vector<float> button_vertices = {
			m_pos.x,	 	     m_pos.y,			  m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,
			m_pos.x + m_width.x, m_pos.y,			  m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,
			m_pos.x + m_width.x, m_pos.y + m_width.y, m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,

			m_pos.x,             m_pos.y,			  m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,
			m_pos.x,			 m_pos.y + m_width.y, m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,
			m_pos.x + m_width.x, m_pos.y + m_width.y, m_color.r, m_color.g, m_color.b, 0.0f, 0.0f, 0.0f,
		};

		m_mesh.insert(m_mesh.end(), button_vertices.begin(), button_vertices.end());
	}

	void hover()
	{
		m_color = glm::vec3(0.6f);
	}

	void click()
	{
		m_color = glm::vec3(0.4f);
	}

	void release()
	{
		m_color = glm::vec3(0.2f);
		action();
	}

	void reset()
	{
		m_color = glm::vec3(1.0f);
	}
};

class UI
{
public:
	std::vector<float> m_mesh;
	std::vector<Button> m_buttons;
	std::vector<Label> m_labels;

	void generateFont()
	{
		for (unsigned int c = 0; c < 256; c++)
		{
			float d = 1.0f / 16.0f;
			float x = (c % 16) * d;
			float y = (16 - c / 16) * d + d;

			Glyph glyph = {
				glm::vec2(x, y),
				glm::vec2(d, d),
				1.0f, // TODO: Read width from csv
			};

			Glyphs.insert(std::pair<int, Glyph>(c, glyph));
		}
	}

	void createMenu()
	{
		Button start_button{};
		start_button.m_pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f - 300);
		start_button.m_width = glm::vec2(800, 200);
		start_button.m_color = glm::vec3(1.0f);
		start_button.action = []() {
			game_state = 1;
		};
		m_buttons.push_back(start_button);

		Button quit_button{};
		quit_button.m_pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f + 100);
		quit_button.m_width = glm::vec2(800, 200);
		quit_button.m_color = glm::vec3(1.0f);
		quit_button.action = []() {
			glfwSetWindowShouldClose(window, true);
		};
		m_buttons.push_back(quit_button);

		Label start_label{};
		start_label.m_pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f - 300);
		start_label.m_color = glm::vec3(0.0f);
		start_label.m_scale = 100.0f;
		start_label.m_text = "Start";
		m_labels.push_back(start_label);

		Label quit_label{};
		quit_label.m_pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f + 100);
		quit_label.m_color = glm::vec3(0.0f);
		quit_label.m_scale = 100.0f;
		quit_label.m_text = "Quit";
		m_labels.push_back(quit_label);
	}

	void updateMenu()
	{
		updateCursor();
		generateMenuMesh();
		updateVAO();
	}

	void updateCursor()
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		for (int i = 0; i < m_buttons.size(); i++)
		{
			if ((xpos >= m_buttons[i].m_pos.x) && (xpos <= m_buttons[i].m_pos.x + m_buttons[i].m_width.x) &&
				(ypos >= m_buttons[i].m_pos.y) && (ypos <= m_buttons[i].m_pos.y + m_buttons[i].m_width.y))
			{
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				{
					m_buttons[i].click();
					m_buttons[i].m_clicked = true;
				}
				else
				{
					if (m_buttons[i].m_clicked)
					{
						m_buttons[i].release();
						m_buttons[i].m_clicked = false;
					}
					else
					{
						m_buttons[i].hover();
					}
				}
			}
			else
			{
				m_buttons[i].reset();
				m_buttons[i].m_clicked = false;
			}
		}
	}

	void generateMenuMesh()
	{
		m_mesh = {};

		for (int i = 0; i < m_buttons.size(); i++)
		{
			m_buttons[i].generateMesh();
			m_mesh.insert(m_mesh.end(), m_buttons[i].m_mesh.begin(), m_buttons[i].m_mesh.end());
		}

		for (int i = 0; i < m_labels.size(); i++)
		{
			m_labels[i].generateMesh();
			m_mesh.insert(m_mesh.end(), m_labels[i].m_mesh.begin(), m_labels[i].m_mesh.end());
		}
	}

	void updateHud()
	{
		if (current_type != last_type)
		{
			generateHudMesh();
			updateVAO();
		}
		last_type = current_type;
	}

	void generateHudMesh()
	{
		m_mesh = {};

		std::vector<float> crosshair_vertices = {
			// pos.x, pos.y, color.r, color.g, color.b, tex.x, tex.y, tex_type
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_LENGTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_WIDTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			WINDOW_WIDTH / 2.0f + CROSSHAIR_WIDTH, WINDOW_HEIGHT / 2.0f + CROSSHAIR_LENGTH, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		};

		float atlas_y = ATLAS_SIZE_Y - current_type;

		std::vector<float> block_icon_vertices = {
			// pos.x, pos.y, color.r, color.g, color.b, tex.x, tex.y, tex_type -> use texture if > 0
			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,

			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
			BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
			BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
		};

		m_mesh.insert(m_mesh.end(), crosshair_vertices.begin(), crosshair_vertices.end());
		m_mesh.insert(m_mesh.end(), block_icon_vertices.begin(), block_icon_vertices.end());
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
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (color)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (texture)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertex attribute (type)
		glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
};