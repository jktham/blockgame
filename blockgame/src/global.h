#pragma once

// settings
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

// camera
float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;

bool first_mouse = true;
bool wireframe_mode = false;

// world
const unsigned int TERRAIN_SEED = 123456;

constexpr glm::ivec2 WORLD_SIZE = glm::ivec2(9, 9); // should be odd, larger than 1
constexpr glm::ivec3 CHUNK_SIZE = glm::ivec3(16, 16, 64); // weird behavior <= 8, investigate

glm::vec2 current_chunk = glm::vec2(0.0f, 0.0f);
glm::vec2 last_chunk = glm::vec2(0.0f, 0.0f);

std::vector<glm::vec3> exposed_blocks;

// rendering
unsigned int VAO;
unsigned int VBO;

// timing
float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
const float FRAME_RATE_LIMIT = 120.0f;