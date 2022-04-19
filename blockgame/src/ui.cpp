#include "ui.h"
#include "player.h"
#include "game.h"
#include "inventory.h"
#include "console.h"
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

void Rectangle::generateMesh()
{
	mesh = {};

	std::vector<float> vertices = {
		pos.x,	 	     pos.y,			  color.r, color.g, color.b, color.a, tex_pos.x,			   tex_pos.y + tex_width.y, (float)tex_mode,
		pos.x + width.x, pos.y,			  color.r, color.g, color.b, color.a, tex_pos.x + tex_width.x, tex_pos.y + tex_width.y, (float)tex_mode,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, color.a, tex_pos.x + tex_width.x, tex_pos.y,				(float)tex_mode,

		pos.x,           pos.y,			  color.r, color.g, color.b, color.a, tex_pos.x,			   tex_pos.y + tex_width.y,	(float)tex_mode,
		pos.x,			 pos.y + width.y, color.r, color.g, color.b, color.a, tex_pos.x,			   tex_pos.y,				(float)tex_mode,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, color.a, tex_pos.x + tex_width.x, tex_pos.y,				(float)tex_mode,
	};

	mesh.insert(mesh.end(), vertices.begin(), vertices.end());
}

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
			x,     y + h, color.r, color.g, color.b, color.a, ch.tex_pos.x,				     ch.tex_pos.y,				    2.0f,
			x,     y,     color.r, color.g, color.b, color.a, ch.tex_pos.x,				     ch.tex_pos.y + ch.tex_width.y, 2.0f,
			x + w, y,     color.r, color.g, color.b, color.a, ch.tex_pos.x + ch.tex_width.x, ch.tex_pos.y + ch.tex_width.y, 2.0f,

			x,     y + h, color.r, color.g, color.b, color.a, ch.tex_pos.x,				     ch.tex_pos.y,				    2.0f,
			x + w, y,     color.r, color.g, color.b, color.a, ch.tex_pos.x + ch.tex_width.x, ch.tex_pos.y + ch.tex_width.y, 2.0f,
			x + w, y + h, color.r, color.g, color.b, color.a, ch.tex_pos.x + ch.tex_width.x, ch.tex_pos.y,				    2.0f,
		};

		x += ch.width * scale;

		mesh.insert(mesh.end(), character_vertices.begin(), character_vertices.end());
	}
}

void Button::generateMesh()
{
	mesh = {};

	std::vector<float> vertices = {
		pos.x,	 	     pos.y,			  color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y,			  color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,

		pos.x,           pos.y,			  color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,
		pos.x,			 pos.y + width.y, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,
		pos.x + width.x, pos.y + width.y, color.r, color.g, color.b, color.a, 0.0f, 0.0f, 0.0f,
	};

	mesh.insert(mesh.end(), vertices.begin(), vertices.end());
}

void Button::hover()
{
	color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
}

void Button::click()
{
	color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
}

void Button::release()
{
	color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	action();
}

void Button::reset()
{
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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
	start_button.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	start_button.action = []() {
		game->start();
	};
	buttons.push_back(start_button);

	Button quit_button{};
	quit_button.pos = glm::vec2(WINDOW_WIDTH / 2.0f - 400, WINDOW_HEIGHT / 2.0f + 100);
	quit_button.width = glm::vec2(800, 200);
	quit_button.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	quit_button.action = []() {
		glfwSetWindowShouldClose(window, true);
	};
	buttons.push_back(quit_button);

	Label start_label{};
	start_label.pos = glm::vec2(start_button.pos.x, start_button.pos.y);
	start_label.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	start_label.scale = 80.0f;
	start_label.text = "Start";
	labels.push_back(start_label);

	Label quit_label{};
	quit_label.pos = glm::vec2(quit_button.pos.x, quit_button.pos.y);
	quit_label.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	quit_label.scale = 80.0f;
	quit_label.text = "Quit";
	labels.push_back(quit_label);

	Label info_label{};
	info_label.pos = glm::vec2(WINDOW_WIDTH - 450, WINDOW_HEIGHT / 2.0f - 300);
	info_label.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	info_label.scale = 30.0f;
	info_label.text = "WASD: Movement§SHIFT: Sprint§SPACE: Jump§§M1/J: Break Block§M2/L: Place Block / Use Item§M3/K: Pick Block§MW/0-9: Select Slot§G: Drop Slot§§N: Save World§M: Load World§R: Reset World§§Q: Toggle Noclip§F: Toggle Fog§TAB: Toggle Wireframe§§ENTER: Open Console§ESCAPE: Quit";
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
	generateHudMesh();
	updateVAO();
}

void UI::generateHudMesh()
{
	mesh = {};

	Rectangle crosshair_h{};
	crosshair_h.pos = glm::vec2(WINDOW_WIDTH / 2.0f - CROSSHAIR_LENGTH / 2.0f, WINDOW_HEIGHT / 2.0f - CROSSHAIR_WIDTH / 2.0f);
	crosshair_h.width = glm::vec2(CROSSHAIR_LENGTH, CROSSHAIR_WIDTH);
	crosshair_h.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	crosshair_h.generateMesh();
	mesh.insert(mesh.end(), crosshair_h.mesh.begin(), crosshair_h.mesh.end());

	Rectangle crosshair_v{};
	crosshair_v.pos = glm::vec2(WINDOW_WIDTH / 2.0f - CROSSHAIR_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - CROSSHAIR_LENGTH / 2.0f);
	crosshair_v.width = glm::vec2(CROSSHAIR_WIDTH, CROSSHAIR_LENGTH);
	crosshair_v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	crosshair_v.generateMesh();
	mesh.insert(mesh.end(), crosshair_v.mesh.begin(), crosshair_v.mesh.end());

	for (int i = 0; i < 10; i++)
	{
		Rectangle slot{};
		slot.pos = glm::vec2(HOTBAR_POS_X + SLOT_GAP / 2.0f - (5 - i) * (SLOT_WIDTH + SLOT_GAP), HOTBAR_POS_Y);
		slot.width = glm::vec2(SLOT_WIDTH, SLOT_WIDTH);
		slot.color = (i == inventory->current_slot) ? glm::vec4(0.1f, 0.1f, 0.1f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.5f);
		slot.generateMesh();
		mesh.insert(mesh.end(), slot.mesh.begin(), slot.mesh.end());

		if (inventory->slots[i].id != 0)
		{
			Rectangle slot_icon{};
			slot_icon.pos = glm::vec2(slot.pos.x + (SLOT_WIDTH - SLOT_ICON_WIDTH) / 2.0f, slot.pos.y + (SLOT_WIDTH - SLOT_ICON_WIDTH) / 2.0f);
			slot_icon.width = glm::vec2(SLOT_ICON_WIDTH, SLOT_ICON_WIDTH);
			slot_icon.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			slot_icon.tex_pos = glm::vec2(0.0f, (float)(atlas_height - inventory->slots[i].id) / atlas_height);
			slot_icon.tex_width = glm::vec2(1.0f / atlas_width, 1.0f / atlas_height);
			slot_icon.tex_mode = 1;
			slot_icon.generateMesh();
			mesh.insert(mesh.end(), slot_icon.mesh.begin(), slot_icon.mesh.end());
		}

		if (inventory->slots[i].amount > 0)
		{
			Label slot_label{};
			slot_label.pos = glm::vec2(HOTBAR_POS_X + SLOT_GAP / 2.0f - (5 - i) * (SLOT_WIDTH + SLOT_GAP), HOTBAR_POS_Y + SLOT_WIDTH - 30.0f);
			slot_label.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			slot_label.scale = 30.0f;
			slot_label.text = std::to_string(inventory->slots[i].amount);
			slot_label.generateMesh();
			mesh.insert(mesh.end(), slot_label.mesh.begin(), slot_label.mesh.end());
		}
	}

	if (inventory->slots[inventory->current_slot].id != 0)
	{
		Rectangle held_item{};
		held_item.pos = glm::vec2(HELD_ITEM_POS_X, HELD_ITEM_POS_Y);
		held_item.width = glm::vec2(HELD_ITEM_WIDTH, HELD_ITEM_WIDTH);
		held_item.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		held_item.tex_pos = glm::vec2(0.0f, (float)(atlas_height - inventory->slots[inventory->current_slot].id) / atlas_height);
		held_item.tex_width = glm::vec2(1.0f / atlas_width, 1.0f / atlas_height);
		held_item.tex_mode = 1;
		held_item.generateMesh();
		mesh.insert(mesh.end(), held_item.mesh.begin(), held_item.mesh.end());

		Label held_item_label{};
		held_item_label.pos = glm::vec2(HELD_ITEM_POS_X, HELD_ITEM_POS_Y - 30.0f);
		held_item_label.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		held_item_label.scale = 30.0f;
		held_item_label.text = inventory->items[inventory->slots[inventory->current_slot].id].name;
		held_item_label.generateMesh();
		mesh.insert(mesh.end(), held_item_label.mesh.begin(), held_item_label.mesh.end());
	}
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (color)
	glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (texcoord)
	glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertex attribute (mode)
	glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void UI::updateConsole()
{
	mesh = {};

	Label console_label{};
	console_label.pos = glm::vec2(0.0f, 0.0f);
	console_label.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	console_label.scale = 40.0f;
	console_label.text = console->entry + "_";
	console_label.generateMesh();
	mesh.insert(mesh.end(), console_label.mesh.begin(), console_label.mesh.end());

	std::string id_names;
	for (int i = 0; i < inventory->items.size(); i++)
	{
		id_names += std::to_string(i) + ": " + inventory->items[i].name + "§";
	}

	Label info_label{};
	info_label.pos = glm::vec2(0.0f, 100.0f);
	info_label.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	info_label.scale = 30.0f;
	info_label.text = "Commands:§GIVE id amount§TAKE id amount§PLACE x y z id§BREAK x y z§§IDs:§" + id_names;
	info_label.generateMesh();
	mesh.insert(mesh.end(), info_label.mesh.begin(), info_label.mesh.end());

	updateVAO();
}