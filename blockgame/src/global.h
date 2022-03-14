#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <deque>

class Game;
class Terrain;
class World;
class Camera;
class Light;
class UI;

extern Game* game;
extern Terrain* terrain;
extern World* world;
extern Camera* camera;
extern Light* light;
extern UI* ui;

extern GLFWwindow* window;

extern const unsigned int WINDOW_WIDTH;
extern const unsigned int WINDOW_HEIGHT;

extern float last_x;
extern float last_y;

extern bool first_mouse;
extern bool wireframe_mode;

extern const int CLICK_DELAY;
extern int left_delay;
extern int right_delay;

extern const int MAX_TYPE;
extern int current_type;
extern int last_type;

extern const float ATLAS_SIZE_X;
extern const float ATLAS_SIZE_Y;

extern const float CROSSHAIR_WIDTH;
extern const float CROSSHAIR_LENGTH;

extern const float BLOCK_ICON_POS_X;
extern const float BLOCK_ICON_POS_Y;
extern const float BLOCK_ICON_WIDTH;

extern float delta_time;
extern float current_frame;
extern float last_frame;
extern float frame_rate;
extern std::deque<float> past_frames;
extern const float FRAME_RATE_LIMIT;

extern const unsigned int TERRAIN_SEED;

inline constexpr glm::ivec2 WORLD_SIZE = glm::ivec2(9, 9); // must be odd, larger than 1
inline constexpr glm::ivec3 CHUNK_SIZE = glm::ivec3(16, 16, 64);

extern glm::vec2 current_chunk;
extern glm::vec2 last_chunk;

extern std::vector<glm::vec3> exposed_blocks;
extern std::vector<glm::vec3> reachable_blocks;
extern std::vector<glm::vec3> collision_blocks_v;
extern std::vector<glm::vec3> collision_blocks_h;
extern std::vector<glm::vec2> generated_chunks;

extern unsigned int world_VAO;
extern unsigned int world_VBO;

extern unsigned int ui_VAO;
extern unsigned int ui_VBO;
