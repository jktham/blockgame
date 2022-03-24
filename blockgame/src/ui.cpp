#include "ui.h"
#include "player.h"
#include "game.h"
#include "global.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <vector>
#include <functional>
#include <map>
#include <iostream>

void Label::generateMesh()
{
	mesh = {};

	std::string newline = "§";
	float x = pos.x;
	float y = pos.y;

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		if (*c == *(newline.c_str()))
		{
			x = pos.x;
			y += 1.0f * scale;
			continue;
		}

		Glyph ch = ui->glyphs[*c];

		float w = 1.0f * scale;
		float h = 1.0f * scale;

		std::vector<float> character_vertices = {
			x,     y + h, color.r, color.g, color.b, ch.tex_coord.x,				 ch.tex_coord.y,				 2.0f,
			x,     y,     color.r, color.g, color.b, ch.tex_coord.x,				 ch.tex_coord.y + ch.tex_size.y, 2.0f,
			x + w, y,     color.r, color.g, color.b, ch.tex_coord.x + ch.tex_size.x, ch.tex_coord.y + ch.tex_size.y, 2.0f,

			x,     y + h, color.r, color.g, color.b, ch.tex_coord.x,				 ch.tex_coord.y,				 2.0f,
			x + w, y,     color.r, color.g, color.b, ch.tex_coord.x + ch.tex_size.x, ch.tex_coord.y + ch.tex_size.y, 2.0f,
			x + w, y + h, color.r, color.g, color.b, ch.tex_coord.x + ch.tex_size.x, ch.tex_coord.y,				 2.0f,
		};

		x += ch.width * scale;

		mesh.insert(mesh.end(), character_vertices.begin(), character_vertices.end());
	}
}

void Button::generateMesh()
{
	mesh = {};

	std::vector<float> button_vertices = {
		pos.x,	 	     pos.y,			  color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y,			  color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,

		pos.x,             pos.y,			  color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,
		pos.x,			 pos.y + width.y, color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, 0.0f, 0.0f, 0.0f,
	};

	mesh.insert(mesh.end(), button_vertices.begin(), button_vertices.end());
}

void Button::hover()
{
	color = glm::vec3(0.6f);
}

void Button::click()
{
	color = glm::vec3(0.4f);
}

void Button::release()
{
	color = glm::vec3(0.2f);
	action();
}

void Button::reset()
{
	color = glm::vec3(1.0f);
}

void UI::generateFont()
{
	std::fstream file("res/fonts/arial.csv", std::fstream::in);
	std::string line;
	std::vector<std::string> lines;
	std::vector<std::string> values;
	std::vector<float> widths;

	while (std::getline(file, line, '\n'))
	{
		lines.push_back(line);
	}

	file.close();

	for (int i = 0; i < lines.size(); i++)
	{
		values.push_back(lines[i].substr(lines[i].find(",") + 1));

		if (i > 7 && i < 264)
		{
			widths.push_back(std::stof(values[i]) / std::stof(values[2]));
		}
	}

	for (unsigned int c = 0; c < 256; c++)
	{
		float w = 1.0f / 16.0f;
		float x = (c % 16) * w;
		float y = (float)(16 - c / 16) * w + w;

		Glyph glyph = {
			glm::vec2(x, y),
			glm::vec2(w),
			widths[c],
		};

		glyphs.insert(std::pair<int, Glyph>(c, glyph));
	}
}

void UI::createMenu()
{
	Button start_button{};
	start_button.pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f - 300);
	start_button.width = glm::vec2(800, 200);
	start_button.color = glm::vec3(1.0f);
	start_button.action = []() {
		game->start();
	};
	buttons.push_back(start_button);

	Button quit_button{};
	quit_button.pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f + 100);
	quit_button.width = glm::vec2(800, 200);
	quit_button.color = glm::vec3(1.0f);
	quit_button.action = []() {
		glfwSetWindowShouldClose(window, true);
	};
	buttons.push_back(quit_button);

	Label start_label{};
	start_label.pos = glm::vec2(start_button.pos.x, start_button.pos.y);
	start_label.color = glm::vec3(0.0f);
	start_label.scale = 80.0f;
	start_label.text = "Start";
	labels.push_back(start_label);

	Label quit_label{};
	quit_label.pos = glm::vec2(quit_button.pos.x, quit_button.pos.y);
	quit_label.color = glm::vec3(0.0f);
	quit_label.scale = 80.0f;
	quit_label.text = "Quit";
	labels.push_back(quit_label);

	Label info_label{};
	info_label.pos = glm::vec2(WINDOW_WIDTH - 450, WINDOW_HEIGHT / 2.0f - 300);
	info_label.color = glm::vec3(1.0f);
	info_label.scale = 36.0f;
	info_label.text = "WASD: Movement§SHIFT: Sprint§SPACE: Jump§§M1/J: Place Block§M2/L: Break Block§M3/K: Pick Block§MW: Change Type§§N: Save World§M: Load World§R: Reset World§§Q: Toggle Noclip§F: Toggle Fog§TAB: Toggle Wireframe§§ENTER: Start§ESCAPE: Quit";
	labels.push_back(info_label);
}

void UI::updateMenu()
{
	updateCursor();
	generateMenuMesh();
	updateVAO();
}

void UI::updateCursor()
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	for (int i = 0; i < buttons.size(); i++)
	{
		if ((xpos >= buttons[i].pos.x) && (xpos <= buttons[i].pos.x + buttons[i].width.x) &&
			(ypos >= buttons[i].pos.y) && (ypos <= buttons[i].pos.y + buttons[i].width.y))
		{
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				buttons[i].click();
				buttons[i].clicked = true;
			}
			else
			{
				if (buttons[i].clicked)
				{
					buttons[i].release();
					buttons[i].clicked = false;
				}
				else
				{
					buttons[i].hover();
				}
			}
		}
		else
		{
			buttons[i].reset();
			buttons[i].clicked = false;
		}
	}
}

void UI::generateMenuMesh()
{
	mesh = {};

	for (int i = 0; i < buttons.size(); i++)
	{
		buttons[i].generateMesh();
		mesh.insert(mesh.end(), buttons[i].mesh.begin(), buttons[i].mesh.end());
	}

	for (int i = 0; i < labels.size(); i++)
	{
		labels[i].generateMesh();
		mesh.insert(mesh.end(), labels[i].mesh.begin(), labels[i].mesh.end());
	}
}

void UI::updateHud()
{
	if (player->current_type != player->last_type)
	{
		generateHudMesh();
		updateVAO();
	}
	player->last_type = player->current_type;
}

void UI::generateHudMesh()
{
	mesh = {};

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

	float atlas_y = ATLAS_SIZE_Y - player->current_type;

	std::vector<float> block_icon_vertices = {
		// pos.x, pos.y, color.r, color.g, color.b, tex.x, tex.y, tex_type -> use texture if > 0
		BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
		BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
		BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,

		BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 0.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (1.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
		BLOCK_ICON_POS_X + 0.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 0.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
		BLOCK_ICON_POS_X + 1.0f * BLOCK_ICON_WIDTH, BLOCK_ICON_POS_Y + 1.0f * BLOCK_ICON_WIDTH, 0.0f, 0.0f, 0.0f, 1.0f / ATLAS_SIZE_X, (0.0f + atlas_y) / ATLAS_SIZE_Y, 1.0f,
	};

	mesh.insert(mesh.end(), crosshair_vertices.begin(), crosshair_vertices.end());
	mesh.insert(mesh.end(), block_icon_vertices.begin(), block_icon_vertices.end());
}

void UI::updateVAO()
{
	// vertex array object
	glBindVertexArray(ui_VAO);

	// vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(), GL_STATIC_DRAW);
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