#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "global.h"
#include "camera.h"
#include "player.h"
#include "world.h"

std::tuple<glm::vec3, glm::vec3> Camera::getRayIntersect()
{
	world->reachable_blocks = {};
	for (int i = 0; i < world->exposed_blocks.size(); i += 1)
	{
		if (glm::length(position - world->exposed_blocks[i]) < ray_length + 1.0f)
		{
			world->reachable_blocks.push_back(world->exposed_blocks[i]);
		}
	}

	for (float r = 0; r < ray_length; r += ray_step)
	{
		glm::vec3 ray = player->front * r;

		for (int i = 0; i < world->reachable_blocks.size(); i++)
		{
			glm::vec3 block_collision_min = world->reachable_blocks[i];
			glm::vec3 block_collision_max = world->reachable_blocks[i] + glm::vec3(1.0f);

			if ((position.x + ray.x <= block_collision_max.x && position.x + ray.x >= block_collision_min.x) &&
				(position.y + ray.y <= block_collision_max.y && position.y + ray.y >= block_collision_min.y) &&
				(position.z + ray.z <= block_collision_max.z && position.z + ray.z >= block_collision_min.z))
			{
				return std::make_tuple(world->reachable_blocks[i], position + ray - (glm::vec3(world->reachable_blocks[i]) + glm::vec3(0.5f)));
			}
		}
	}
	return std::make_tuple(glm::vec3(-1.0f), glm::vec3(-1.0f));
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + player->front, player->up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
}

void Camera::updateCameraVectors()
{
	position = player->position + glm::vec3(0.0f, 0.0f, player->height);

	glm::vec3 view{};
	view.x = sin(glm::radians(player->yaw)) * cos(glm::radians(player->pitch));
	view.y = cos(glm::radians(player->yaw)) * cos(glm::radians(player->pitch));
	view.z = sin(glm::radians(player->pitch));
	player->front = glm::normalize(view);

	player->right = glm::normalize(glm::cross(player->front, glm::vec3(0.0f, 0.0f, 1.0f)));
	player->up = glm::normalize(glm::cross(player->right, player->front));
	player->front_plane = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), player->right));
}