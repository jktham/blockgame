#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SHIFT_SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;

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

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void processKeyboard(Camera_Movement direction, float delta_time)
	{
		float velocity = m_speed * delta_time;
		if (direction == Camera_Movement::FORWARD)
			m_position += m_front_plane * velocity;
		if (direction == Camera_Movement::BACKWARD)
			m_position -= m_front_plane * velocity;
		if (direction == Camera_Movement::LEFT)
			m_position -= m_right * velocity;
		if (direction == Camera_Movement::RIGHT)
			m_position += m_right * velocity;
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