#include "global.h"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

Game* game;
Terrain* terrain;
World* world;
Inventory* inventory;
Player* player;
Camera* camera;
Light* light;
Fog* fog;
UI* ui;
GLFWwindow* window;
Threadpool* threadpool;

const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;

bool first_mouse = true;
bool wireframe_mode = false;
bool first_frame = true;

const int CLICK_DELAY = 30;

const int MAX_TYPE = 5;

const int ATLAS_SIZE_X = 6;
const int ATLAS_SIZE_Y = 7;

float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
float frame_rate = 0.0f;
std::deque<float> past_frames = {};
const float FRAME_RATE_LIMIT = 120.0f;

unsigned int world_VAO;
unsigned int world_VBO;

unsigned int ui_VAO;
unsigned int ui_VBO;