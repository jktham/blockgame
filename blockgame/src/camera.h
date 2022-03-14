#pragma once

#include <glm/glm.hpp>

class Camera
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
	float fov = 80.0f;

	float height = 1.75f;
	float width = 0.25f;

	float gravity = 10.0f;
	float vertical_velocity = 0.0f;

	float ray_length = 8.0f;
	float ray_step = 0.1f;

	bool noclip = false;

	Camera();

	void reset();

	void applyGravity();

	void applyMovement(glm::vec3 direction);

	void applyJump();

	bool detectCollisionV();

	bool detectCollisionH();

	std::tuple<glm::vec3, glm::vec3> getRayIntersect();

	glm::mat4 getViewMatrix();

	glm::mat4 getProjectionMatrix();

	void processKeyboard(int key);

	void processMouseMovement(float offset_x, float offset_y);

private:
	void updateCameraVectors();

	int divideInt(float a, float b);
};