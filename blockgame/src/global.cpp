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
Console* console;

const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

float last_x = WINDOW_WIDTH / 2.0f;
float last_y = WINDOW_HEIGHT / 2.0f;

bool first_mouse = true;
bool wireframe_mode = false;
bool first_frame = true;

int click_delay = 30;

int atlas_width = 0;
int atlas_height = 0;

float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;
float frame_rate = 0.0f;
std::deque<float> past_frames = {};
float frame_rate_limit = 120.0f;

unsigned int world_VAO;
unsigned int world_VBO;

unsigned int ui_VAO;
unsigned int ui_VBO;