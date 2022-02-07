﻿#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	JUMP
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SHIFT_SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;
const float GRAVITY = 10.0f;

class Camera
{
public:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_front_plane;
	glm::vec3 m_up;
	glm::vec3 m_right;

	float m_yaw;
	float m_pitch;

	float m_speed;
	float m_sensitivity;
	float m_fov;

	float m_height = 1.75f;
	float m_width = 0.5f;

	float m_vertical_velocity;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
	{
		m_position = position;
		m_up = up;
		m_yaw = yaw;
		m_pitch = pitch;
		m_speed = SPEED;
		m_sensitivity = SENSITIVITY;
		m_fov = FOV;
		updateCameraVectors();
	}

	void applyGravity(float delta_time)
	{
		float floor_height = 0.0f;

		if (!checkCollisionVertical(floor_height))
		{
			m_vertical_velocity -= GRAVITY * delta_time;
		}
		else
		{
			m_vertical_velocity = 0.0f;
			m_position.y = floor_height + m_height;
		}

		m_position += glm::vec3(0.0f, 1.0f, 0.0f) * m_vertical_velocity * delta_time;

		if (m_vertical_velocity <= -50.0f)
		{
			m_vertical_velocity = -50.0f;
		}

		if (m_position.y < -50.0f)
		{
			m_position.y = 50.0f;
		}
	}

	void applyMovement(glm::vec3 direction, float delta_time)
	{
		glm::vec3 collision_position = glm::vec3(0.0f);

		if (!checkCollisionHorizontal(collision_position))
		{
			float velocity = m_speed * delta_time;
			m_position += direction * velocity;
		}
		else
		{
			glm::vec3 collision_cross = glm::cross(glm::normalize(collision_position), glm::normalize(direction));
			glm::vec3 collision_direction = glm::vec3(glm::vec4(collision_cross, 1.0f) * glm::rotate(glm::mat4(1.0f), 3.1415f / 2.0f, collision_position));
			float collision_velocity = glm::length(collision_cross) * m_speed * delta_time;

			if (glm::dot(glm::normalize(collision_position), glm::normalize(direction)) <= 0.0f)
			{
				//m_position += collision_direction * collision_velocity;
			}
			else
			{
				float velocity = m_speed * delta_time;
				m_position += direction * velocity;
			}

		}
	}

	void applyJump()
	{
		float floor_height = 0.0f;

		if (checkCollisionVertical(floor_height))
		{
			m_vertical_velocity = 5.0f;
			m_position.y += 0.01f;
		}
	}

	bool checkCollisionVertical(float& floor_height)
	{
		for (int i = 0; i < m_offsets.size(); i += 1)
		{
			glm::vec3 block_collision_min = m_offsets[i];
			glm::vec3 block_collision_max = m_offsets[i] + glm::vec3(1.0f);

			glm::vec3 camera_collision_min = m_position + glm::vec3(-m_width, -m_height, -m_width);
			glm::vec3 camera_collision_max = m_position + glm::vec3(m_width, 0.0f, m_width);

			if ((camera_collision_min.x <= block_collision_max.x && camera_collision_max.x >= block_collision_min.x) &&
				(camera_collision_min.y <= block_collision_max.y && camera_collision_max.y >= block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				glm::vec3 collision_position = camera_collision_min - block_collision_min;


				if (collision_position.y > abs(collision_position.x) && collision_position.y > abs(collision_position.z))
				{
					floor_height = block_collision_max.y;

					//std::cout << collision_position.x << ", " << collision_position.y << ", " << collision_position.z << "\n";
					std::cout << "vertical collision\n";
					return true;
				}
			}
		}

		return false;
	}

	bool checkCollisionHorizontal(glm::vec3& collision_position)
	{
		for (int i = 0; i < m_offsets.size(); i += 1)
		{
			glm::vec3 block_collision_min = m_offsets[i];
			glm::vec3 block_collision_max = m_offsets[i] + glm::vec3(1.0f);

			glm::vec3 camera_collision_min = m_position + glm::vec3(-m_width, -m_height, -m_width);
			glm::vec3 camera_collision_max = m_position + glm::vec3(m_width, 0.0f, m_width);

			if ((camera_collision_min.x <= block_collision_max.x && camera_collision_max.x >= block_collision_min.x) &&
				(camera_collision_min.y <= block_collision_max.y && camera_collision_max.y >= block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				collision_position = camera_collision_min - block_collision_min;

				if (collision_position.y < abs(collision_position.x) || collision_position.y < abs(collision_position.z))
				{
					if (glm::abs(collision_position.x) > glm::abs(collision_position.z))
						collision_position = glm::normalize(glm::vec3(collision_position.x, 0.0f, 0.0f));
					else
						collision_position = glm::normalize(glm::vec3(0.0f, 0.0f, collision_position.z));

					//std::cout << collision_position.x << ", " << collision_position.y << ", " << collision_position.z << "\n";
					std::cout << "horizontal collision\n";
					return true;
				}
			}
		}

		return false;
	}

	std::vector<glm::vec3> m_offsets;
	void setOffsets(std::vector<glm::vec3> offsets)
	{
		m_offsets = offsets;
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void processKeyboard(Camera_Movement direction, float delta_time)
	{
		glm::vec3 movement_vector = glm::vec3(0.0f);

		if (direction == Camera_Movement::FORWARD)
			movement_vector += m_front_plane;
		if (direction == Camera_Movement::BACKWARD)
			movement_vector -= m_front_plane;
		if (direction == Camera_Movement::LEFT)
			movement_vector -= m_right;
		if (direction == Camera_Movement::RIGHT)
			movement_vector += m_right;

		applyMovement(movement_vector, delta_time);

		if (direction == Camera_Movement::JUMP)
			applyJump();
	}

	void processMouseMovement(float offset_x, float offset_y, GLboolean constrain_pitch = true)
	{
		offset_x *= m_sensitivity;
		offset_y *= m_sensitivity;

		m_yaw += offset_x;
		m_pitch += offset_y;

		if (constrain_pitch)
		{
			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float offset_y)
	{
		m_fov -= (float)offset_y;
		if (m_fov < 10.0f)
			m_fov = 10.0f;
		if (m_fov > 90.0f)
			m_fov = 90.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front{};
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);

		m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_up = glm::normalize(glm::cross(m_right, m_front));
		m_front_plane = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_right));
	}
};