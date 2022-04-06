#pragma once

#include "terrain.h"
#include "global.h"

#include <glm/glm.hpp>

enum class Action
{
	MOVE_FRONT,
	MOVE_BACK,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_JUMP,
	INTERACT_PRIMARY,
	INTERACT_SECONDARY,
	INTERACT_TERTIARY,
	DROP,
};

class Player
{
public:
	const float SPEED = 7.5f;
	const float GRAVITY = 10.0f;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, terrain->getGroundHeight(0, 0) + 2.0f);
	glm::vec3 up{};
	glm::vec3 front{};
	glm::vec3 front_plane{};
	glm::vec3 front_move{};
	glm::vec3 right{};

	glm::vec2 current_chunk{};
	glm::vec2 last_chunk{};

	std::vector<glm::vec3> collision_blocks_v;
	std::vector<glm::vec3> collision_blocks_h;

	float yaw = 45.0f;
	float pitch = 0.0f;

	float speed = SPEED;
	float sensitivity = 0.1f;

	float height = 1.75f;
	float width = 0.25f;

	float gravity = GRAVITY;
	float vertical_velocity = 0.0f;

	bool noclip = false;
	bool sprint = false;

	int left_delay = 0;
	int right_delay = 0;

	glm::vec3 selected_block{};

	Player();

	void applyGravity();
	void applyMovement(glm::vec3 direction);
	void applyJump();

	bool detectCollisionV();
	bool detectCollisionH();

	void processAction(Action action);
	void processMouseMovement(float offset_x, float offset_y);

	void updateVectors();

private:
	int divideInt(float a, float b);
};