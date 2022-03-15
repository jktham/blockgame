#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>
#include <functional>

struct Glyph {
	glm::vec2 tex_coord;
	glm::vec2 tex_size;
	float width;
};

class Label
{
public:
	glm::vec2 pos = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
	std::vector<float> mesh;
	std::string text = "";
	float scale = 1.0f;

	void generateMesh();
};

class Button
{
public:
	glm::vec2 pos = glm::vec3(0.0f);
	glm::vec2 width = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
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
	const float CROSSHAIR_WIDTH = 1.0f;
	const float CROSSHAIR_LENGTH = 10.0f;

	const float BLOCK_ICON_POS_X = WINDOW_WIDTH - 200.0f;
	const float BLOCK_ICON_POS_Y = WINDOW_HEIGHT - 200.0f;
	const float BLOCK_ICON_WIDTH = 180.0f;

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
};