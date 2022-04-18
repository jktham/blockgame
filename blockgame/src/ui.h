#pragma once

#include "global.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>
#include <functional>

class Rectangle
{
public:
	glm::vec2 pos = glm::vec2(0.0f);
	glm::vec2 width = glm::vec2(0.0f);
	glm::vec4 color = glm::vec4(0.0f);
	glm::vec2 tex_pos = glm::vec2(0.0f);
	glm::vec2 tex_width = glm::vec2(0.0f);
	int tex_mode = 0;
	std::vector<float> mesh;

	void generateMesh();
};

struct Glyph {
	glm::vec2 tex_pos;
	glm::vec2 tex_width;
	float width;
};

class Label
{
public:
	glm::vec2 pos = glm::vec2(0.0f);
	glm::vec4 color = glm::vec4(0.0f);
	std::vector<float> mesh;
	std::string text = "";
	float scale = 1.0f;

	void generateMesh();
};

class Button
{
public:
	glm::vec2 pos = glm::vec2(0.0f);
	glm::vec2 width = glm::vec2(0.0f);
	glm::vec4 color = glm::vec4(0.0f);
	bool clicked = false;
	std::vector<float> mesh;
	std::function<void()> action = []() {};

	void generateMesh();

	void hover();
	void click();
	void release();
	void reset();
};

class UI
{
public:
	const float CROSSHAIR_WIDTH = 2.0f;
	const float CROSSHAIR_LENGTH = 20.0f;

	const float HOTBAR_POS_X = WINDOW_WIDTH / 2.0f;
	const float HOTBAR_POS_Y = WINDOW_HEIGHT - 100.0f;
	const float SLOT_WIDTH = 80.0f;
	const float SLOT_GAP = 10.0f;
	const float SLOT_ICON_WIDTH = 60.0f;

	const float HELD_ITEM_POS_X = WINDOW_WIDTH - 200.0f;
	const float HELD_ITEM_POS_Y = WINDOW_HEIGHT - 200.0f;
	const float HELD_ITEM_WIDTH = 180.0f;

	std::vector<float> mesh;
	std::vector<Button> buttons;
	std::vector<Label> labels;
	std::map<int, Glyph> glyphs;

	void generateFont();
	void createMenu();

	void updateMenu();
	void updateCursor();
	void generateMenuMesh();

	void updateHud();
	void generateHudMesh();

	void updateVAO();

	void updateConsole();
};