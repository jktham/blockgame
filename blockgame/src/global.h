#pragma once

// settings
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

// camera
float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;
bool first_mouse = true;
bool wireframe_mode = false;

const float YAW = 45.0f;
const float PITCH = 0.0f;
const float SPEED = 7.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;
const float GRAVITY = 10.0f;

glm::ivec2 current_chunk = glm::ivec2(0, 0);
glm::ivec2 last_chunk = glm::ivec2(0, 0);

// world
constexpr glm::ivec2 WORLD_SIZE = glm::ivec2(9, 9);
constexpr glm::ivec3 CHUNK_SIZE = glm::ivec3(16, 16, 64);

std::vector<glm::vec3> collision_blocks;

// rendering
unsigned int VAO;
unsigned int VBO;

// timing
float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
float frame_rate_limit = 120.0f;