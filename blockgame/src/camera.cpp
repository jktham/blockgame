#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "global.h"
#include "camera.h"
#include "player.h"
#include "world.h"

glm::vec3 Camera::getRayIntersect()
{
	for (float r = 0.0f; r < ray_length; r += ray_step)
	{
		glm::vec3 ray = player->front * r;

		if (world->getBlockType(position + ray) != 0)
		{
			return position + ray;
		}
	}
	return glm::vec3(-1.0f);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + player->front, player->up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
}

void Camera::updatePosition()
{
	position = player->position + glm::vec3(0.0f, 0.0f, player->height);
}