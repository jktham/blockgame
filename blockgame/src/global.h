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

glm::vec2 current_chunk = glm::vec2(4, 4);

// world
constexpr int WORLD_SIZE[2] = { 9, 9 };
constexpr int CHUNK_SIZE[3] = { 16, 16, 64 };

// timing
float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
float frame_rate_limit = 120.0f;