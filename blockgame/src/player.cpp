#include "player.h"
#include "world.h"
#include "camera.h"
#include "inventory.h"
#include "global.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

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

	vertical_velocity -= gravity * delta_time;

	if (detectCollisionV())
	{
		for (int i = 0; i < collision_blocks_v.size(); i++)
		{
			glm::vec3 collision_offset = position - collision_blocks_v[i];

			if (collision_offset.z >= 0.0f)
			{
				vertical_velocity = 0.0f;
				position.z = collision_blocks_v[i].z + 1.0f;
			}
			else
			{
				vertical_velocity = 0.0f;
				position.z -= 0.01f;
			}
		}
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

	float velocity = speed * delta_time;
	position += direction * velocity;

	if (detectCollisionH())
	{
		for (int i = 0; i < collision_blocks_h.size(); i++)
		{
			glm::vec3 collision_offset = position - collision_blocks_h[i];
			glm::vec3 collision_normal = glm::vec3(0.0f);

			collision_normal = (glm::abs(collision_offset.x - 0.5f) > glm::abs(collision_offset.y - 0.5f)) ? glm::normalize(glm::vec3(collision_offset.x - width, 0.0f, 0.0f)) : glm::normalize(glm::vec3(0.0f, collision_offset.y - width, 0.0f));

			if (collision_normal.x > 0.0f || collision_normal.y > 0.0f)
			{
				position += (glm::vec3(1.0f) - collision_offset + glm::vec3(width)) * collision_normal;
			}
			else if (collision_normal.x < 0.0f || collision_normal.y < 0.0f)
			{
				position += (collision_offset + glm::vec3(width)) * collision_normal;
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

		glm::vec3 player_collision_min = position + glm::vec3(-width, -width, 0.0f);
		glm::vec3 player_collision_max = position + glm::vec3(width, width, height);

		if ((player_collision_min.x < block_collision_max.x && player_collision_max.x > block_collision_min.x) &&
			(player_collision_min.y < block_collision_max.y && player_collision_max.y > block_collision_min.y) &&
			(player_collision_min.z <= block_collision_max.z && player_collision_max.z > block_collision_min.z))
		{
			glm::vec3 collision_offset = player_collision_min - block_collision_min + glm::vec3(width - 0.5f);

			if (abs(collision_offset.z) > abs(collision_offset.x) && abs(collision_offset.z) > abs(collision_offset.y))
			{
				collision_blocks_v.push_back(world->exposed_blocks[i]);
				//std::cout << "vertical collision: (" << i << "/" << world->exposed_blocks.size() << ")\n";
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

		glm::vec3 player_collision_min = position + glm::vec3(-width, -width, 0.0f);
		glm::vec3 player_collision_max = position + glm::vec3(width, width, height);

		if ((player_collision_min.x <= block_collision_max.x && player_collision_max.x >= block_collision_min.x) &&
			(player_collision_min.y <= block_collision_max.y && player_collision_max.y >= block_collision_min.y) &&
			(player_collision_min.z <= block_collision_max.z && player_collision_max.z >= block_collision_min.z))
		{
			glm::vec3 collision_offset = player_collision_min - block_collision_min + glm::vec3(width - 0.5f);

			if (collision_offset.z < abs(collision_offset.x) || collision_offset.z < abs(collision_offset.y))
			{
				collision_blocks_h.push_back(world->exposed_blocks[i]);
				//std::cout << "horizontal collision: (" << i << "/" << world->exposed_blocks.size() << ")\n";
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

	if (action == Action::INTERACT_PRIMARY)
		world->breakBlock(camera->getRayIntersect());

	if (action == Action::INTERACT_SECONDARY)
		if (inventory->items[inventory->slots[inventory->current_slot].id].placeable)
			world->placeBlock(camera->getRayIntersect(), inventory->slots[inventory->current_slot].id);

	if (action == Action::INTERACT_TERTIARY)
	{
		int type = world->getBlockType(camera->getRayIntersect());
		inventory->selectItem(type);
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