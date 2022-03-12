#pragma once

class Game;
class Terrain;
class World;
class Camera;
class Light;
class UI;

Game* game;
Terrain* terrain;
World* world;
Camera* camera;
Light* light;
UI* ui;

GLFWwindow* window;

const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;

bool first_mouse = true;
bool wireframe_mode = false;

const int CLICK_DELAY = 30;
int left_delay = 0;
int right_delay = 0;

const int MAX_TYPE = 5;
int current_type = MAX_TYPE;
int last_type = -1;

const float ATLAS_SIZE_X = 6.0f;
const float ATLAS_SIZE_Y = 6.0f;

const float CROSSHAIR_WIDTH = 1.0f;
const float CROSSHAIR_LENGTH = 10.0f;

const float BLOCK_ICON_POS_X = WINDOW_WIDTH - 200.0f;
const float BLOCK_ICON_POS_Y = WINDOW_HEIGHT - 200.0f;
const float BLOCK_ICON_WIDTH = 180.0f;

float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
const float FRAME_RATE_LIMIT = 120.0f;

const unsigned int TERRAIN_SEED = 123456;

constexpr glm::ivec2 WORLD_SIZE = glm::ivec2(9, 9); // must be odd, larger than 1
constexpr glm::ivec3 CHUNK_SIZE = glm::ivec3(16, 16, 64);

glm::vec2 current_chunk = glm::vec2(0.0f, 0.0f);
glm::vec2 last_chunk = glm::vec2(0.0f, 0.0f);

std::vector<glm::vec3> exposed_blocks;
std::vector<glm::vec3> reachable_blocks;
std::vector<glm::vec3> collision_blocks_v;
std::vector<glm::vec3> collision_blocks_h;
std::vector<glm::vec2> generated_chunks;

unsigned int world_VAO;
unsigned int world_VBO;

unsigned int ui_VAO;
unsigned int ui_VBO;
