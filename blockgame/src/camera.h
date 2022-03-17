#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	glm::vec3 position{};

	float fov = 80.0f;

	float ray_length = 8.0f;
	float ray_step = 0.001f;

	// cast ray from camera and return coordinates when ray intersects non air block
	glm::vec3 getRayIntersect();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	void updatePosition();
};