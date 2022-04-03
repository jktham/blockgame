#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <deque>

class Game;
class Terrain;
class World;
class Inventory;
class Player;
class Camera;
class Light;
class Fog;
class UI;
struct GLFWwindow;
class Threadpool;

extern Game* game;
extern Terrain* terrain;
extern World* world;
extern Inventory* inventory;
extern Player* player;
extern Camera* camera;
extern Light* light;
extern Fog* fog;
extern UI* ui;
extern GLFWwindow* window;
extern Threadpool* threadpool;

extern const unsigned int WINDOW_WIDTH;
extern const unsigned int WINDOW_HEIGHT;

extern float last_x;
extern float last_y;

extern bool first_mouse;
extern bool wireframe_mode;
extern bool first_frame;

extern const int CLICK_DELAY;

extern const int ATLAS_SIZE_X;
extern const int ATLAS_SIZE_Y;

extern float delta_time;
extern float current_frame;
extern float last_frame;
extern float frame_rate;
extern std::deque<float> past_frames;
extern const float FRAME_RATE_LIMIT;

inline constexpr glm::ivec2 WORLD_SIZE = glm::ivec2(9, 9); // must be odd, larger than 1
inline constexpr glm::ivec3 CHUNK_SIZE = glm::ivec3(16, 16, 64);

extern unsigned int world_VAO;
extern unsigned int world_VBO;

extern unsigned int ui_VAO;
extern unsigned int ui_VBO;
