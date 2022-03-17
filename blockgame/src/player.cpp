#include "player.h"
#include "world.h"
#include "camera.h"
#include "global.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

Player::Player()
{
	updateVectors();
}

void Player::applyGravity()
{
	if (noclip)
	{
		vertical_velocity = 0.0f;
		return;
	}

	if (!detectCollisionV())
	{
		vertical_velocity -= gravity * delta_time;
	}
	else
	{
		vertical_velocity = 0.0f;
		position.z = collision_blocks_v[0].z + 1.0f;
	}

	position += glm::vec3(0.0f, 0.0f, 1.0f) * vertical_velocity * delta_time;

	if (vertical_velocity <= -50.0f)
		vertical_velocity = -50.0f;
	if (position.z < -32.0f)
		position.z = CHUNK_SIZE.z + 64.0f;
}

void Player::applyMovement(glm::vec3 direction)
{
	if (sprint)
		speed = 7.5f * 5.0f;
	else
		speed = 7.5f;

	if (!detectCollisionH())
	{
		float velocity = speed * delta_time;
		position += direction * velocity;
	}
	else
	{
		for (int i = 0; i < collision_blocks_h.size(); i++) // TODO: consider multiblock collisions
		{
			glm::vec3 collision_offset = position + glm::vec3(-width, -width, 0.0f) - collision_blocks_h[i];
			glm::vec3 collision_normal = glm::vec3(0.0f);

			if (glm::abs(collision_offset.x) > glm::abs(collision_offset.y))
			{
				collision_normal = glm::normalize(glm::vec3(collision_offset.x, 0.0f, 0.0f));
			}
			else
			{
				collision_normal = glm::normalize(glm::vec3(0.0f, collision_offset.y, 0.0f));
			}

			glm::vec3 collision_cross = glm::cross(glm::normalize(collision_normal), glm::normalize(direction));
			glm::vec3 collision_direction = glm::vec3(glm::vec4(collision_cross, 1.0f) * glm::rotate(glm::mat4(1.0f), 3.1415f / 2.0f, collision_normal));
			float collision_velocity = glm::length(collision_cross) * speed * delta_time;

			if (glm::dot(glm::normalize(collision_normal), glm::normalize(direction)) <= 0.0f)
			{
				position += collision_direction * collision_velocity;
			}
			else
			{
				float velocity = speed * delta_time;
				position += direction * velocity;
			}
		}
	}

	current_chunk = glm::vec2(divideInt(position.x, (float)CHUNK_SIZE.x), divideInt(position.y, (float)CHUNK_SIZE.y));
}

void Player::applyJump()
{
	if (detectCollisionV())
	{
		vertical_velocity = 7.5f;
		position.z += 0.01f;
	}
}

bool Player::detectCollisionV()
{
	collision_blocks_v = {};

	if (noclip)
		return false;

	for (int i = 0; i < world->exposed_blocks.size(); i += 1)
	{
		glm::vec3 block_collision_min = world->exposed_blocks[i];
		glm::vec3 block_collision_max = world->exposed_blocks[i] + glm::vec3(1.0f);

		glm::vec3 camera_collision_min = position + glm::vec3(-width, -width, 0.0f);
		glm::vec3 camera_collision_max = position + glm::vec3(width, width, 0.0f);

		if ((camera_collision_min.x < block_collision_max.x && camera_collision_max.x > block_collision_min.x) &&
			(camera_collision_min.y < block_collision_max.y && camera_collision_max.y > block_collision_min.y) &&
			(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
		{
			glm::vec3 collision_offset = camera_collision_min - block_collision_min;

			if (collision_offset.z > abs(collision_offset.x) && collision_offset.z > abs(collision_offset.y))
			{
				collision_blocks_v.push_back(world->exposed_blocks[i]);
				std::cout << "vertical collision: (" << i << "/" << world->exposed_blocks.size() << ")\n";
			}
		}
	}

	if (!collision_blocks_v.empty())
	{
		return true;
	}
	return false;
}

bool Player::detectCollisionH()
{
	collision_blocks_h = {};

	if (noclip)
		return false;

	for (int i = 0; i < world->exposed_blocks.size(); i += 1)
	{
		glm::vec3 block_collision_min = world->exposed_blocks[i];
		glm::vec3 block_collision_max = world->exposed_blocks[i] + glm::vec3(1.0f);

		glm::vec3 camera_collision_min = position + glm::vec3(-width, -width, 0.0f);
		glm::vec3 camera_collision_max = position + glm::vec3(width, width, 0.0f);

		if ((camera_collision_min.x <= block_collision_max.x && camera_collision_max.x >= block_collision_min.x) &&
			(camera_collision_min.y <= block_collision_max.y && camera_collision_max.y >= block_collision_min.y) &&
			(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
		{
			glm::vec3 collision_offset = camera_collision_min - block_collision_min;

			if (collision_offset.z < abs(collision_offset.x) || collision_offset.z < abs(collision_offset.y))
			{
				collision_blocks_h.push_back(world->exposed_blocks[i]);
				std::cout << "horizontal collision: (" << i << "/" << world->exposed_blocks.size() << ")\n";
			}
		}
	}

	if (!collision_blocks_h.empty())
	{
		return true;
	}
	return false;
}

void Player::processAction(Action action)
{
	if (noclip)
		front_move = front;
	else
		front_move = front_plane;

	glm::vec3 movement_vector = glm::vec3(0.0f);

	if (action == Action::MOVE_FRONT)
		movement_vector += front_move;
	if (action == Action::MOVE_BACK)
		movement_vector -= front_move;
	if (action == Action::MOVE_LEFT)
		movement_vector -= right;
	if (action == Action::MOVE_RIGHT)
		movement_vector += right;

	applyMovement(movement_vector);

	if (action == Action::MOVE_JUMP)
		applyJump();

	if (action == Action::BLOCK_DESTROY)
		world->destroyBlock(camera->getRayIntersect());
	if (action == Action::BLOCK_PLACE)
		world->placeBlock(camera->getRayIntersect());
	if (action == Action::BLOCK_PICK)
	{
		int type = world->getBlockType(camera->getRayIntersect());
		if (type != 0)
			current_type = type;
	}
}

void Player::processMouseMovement(float offset_x, float offset_y)
{
	offset_x *= sensitivity;
	offset_y *= sensitivity;

	yaw += offset_x;
	pitch += offset_y;

	if (pitch > 89.9999f)
		pitch = 89.9999f;
	if (pitch < -89.9999f)
		pitch = -89.9999f;

	updateVectors();
}

void Player::updateVectors()
{
	glm::vec3 view{};
	view.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	view.y = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	view.z = sin(glm::radians(pitch));
	front = glm::normalize(view);

	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 0.0f, 1.0f)));
	up = glm::normalize(glm::cross(right, front));
	front_plane = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), right));
}

int Player::divideInt(float a, float b)
{
	if (a < 0) a -= 1;
	return (int)(a / b - ((int)a % (int)b < 0 ? 1 : 0));
}