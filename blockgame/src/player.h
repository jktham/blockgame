#pragma once

#include <glm/glm.hpp>

enum class Action
{
	MOVE_FRONT,
	MOVE_BACK,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_JUMP,
	BLOCK_PLACE,
	BLOCK_DESTROY,
	BLOCK_PICK,
};

class Player
{
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, CHUNK_SIZE.z / 2.0f + 16.0f);
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 front_plane;
	glm::vec3 front_move;
	glm::vec3 right;

	float yaw = 45.0f;
	float pitch = 0.0f;

	float speed = 7.5f;
	float sensitivity = 0.1f;

	float height = 1.75f;
	float width = 0.25f;

	float gravity = 10.0f;
	float vertical_velocity = 0.0f;

	bool noclip = false;
	bool sprint = false;

	void applyGravity();
	void applyMovement(glm::vec3 direction);
	void applyJump();

	bool detectCollisionV();
	bool detectCollisionH();

	void processAction(Action action);
	void processMouseMovement(float offset_x, float offset_y);

private:
	int divideInt(float a, float b);
};