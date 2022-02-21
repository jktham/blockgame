#pragma once

class Camera
{
public:
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, CHUNK_SIZE.z / 2.0f + 16.0f);
	glm::vec3 m_up;
	glm::vec3 m_front;
	glm::vec3 m_front_plane;
	glm::vec3 m_front_move;
	glm::vec3 m_right;

	float m_yaw = 45.0f;
	float m_pitch = 0.0f;

	float m_speed = 7.5f;
	float m_sensitivity = 0.1f;
	float m_fov = 70.0f;

	float m_height = 1.75f;
	float m_width = 0.25f;

	float m_gravity = 10.0f;
	float m_vertical_velocity = 0.0f;

	float m_ray_length = 8.0f;
	float m_ray_step = 0.25f;

	bool m_noclip = false;

	Camera()
	{
		updateCameraVectors();
	}

	void applyGravity()
	{
		if (m_noclip)
		{
			m_vertical_velocity = 0.0f;
			return;
		}

		float floor_height = 0.0f;

		if (!checkCollisionVertical(floor_height))
		{
			m_vertical_velocity -= m_gravity * delta_time;
		}
		else
		{
			m_vertical_velocity = 0.0f;
			m_position.z = floor_height + m_height;
		}

		m_position += glm::vec3(0.0f, 0.0f, 1.0f) * m_vertical_velocity * delta_time;

		if (m_vertical_velocity <= -50.0f)
			m_vertical_velocity = -50.0f;
		if (m_position.z < -32.0f)
			m_position.z = 256.0f;
	}

	void applyMovement(glm::vec3 direction)
	{
		glm::vec3 collision_position = glm::vec3(0.0f);
		glm::vec3 collision_normal = glm::vec3(0.0f);

		if (!checkCollisionHorizontal(collision_position, collision_normal))
		{
			float velocity = m_speed * delta_time;
			m_position += direction * velocity;
		}
		else
		{
			glm::vec3 collision_cross = glm::cross(glm::normalize(collision_normal), glm::normalize(direction));
			glm::vec3 collision_direction = glm::vec3(glm::vec4(collision_cross, 1.0f) * glm::rotate(glm::mat4(1.0f), 3.1415f / 2.0f, collision_normal));
			float collision_velocity = glm::length(collision_cross) * m_speed * delta_time;

			if (glm::dot(glm::normalize(collision_normal), glm::normalize(direction)) <= 0.0f)
			{
				m_position += collision_direction * collision_velocity;
			}
			else
			{
				float velocity = m_speed * delta_time;
				m_position += direction * velocity;
			}
		}

		current_chunk = glm::vec2(divideInt(m_position.x, (float)CHUNK_SIZE.x), divideInt(m_position.y, (float)CHUNK_SIZE.y));
	}

	void applyJump()
	{
		float floor_height = 0.0f;

		if (checkCollisionVertical(floor_height))
		{
			m_vertical_velocity = 7.5f;
			m_position.z += 0.01f;
		}
	}

	bool checkCollisionVertical(float& floor_height)
	{
		if (m_noclip)
			return false;

		for (int i = 0; i < exposed_blocks.size(); i += 1)
		{
			glm::vec3 block_collision_min = exposed_blocks[i];
			glm::vec3 block_collision_max = exposed_blocks[i] + glm::ivec3(1);

			glm::vec3 camera_collision_min = m_position + glm::vec3(-m_width, -m_width, -m_height);
			glm::vec3 camera_collision_max = m_position + glm::vec3(m_width, m_width, 0.0);

			if ((camera_collision_min.x < block_collision_max.x && camera_collision_max.x > block_collision_min.x) &&
				(camera_collision_min.y < block_collision_max.y && camera_collision_max.y > block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				glm::vec3 collision_position = camera_collision_min - block_collision_min;

				if (collision_position.z > abs(collision_position.x) && collision_position.z > abs(collision_position.y))
				{
					floor_height = block_collision_max.z;

					std::cout << "vertical collision: (" << i << "/" << exposed_blocks.size() << ")\n";
					return true;
				}
			}
		}

		return false;
	}

	bool checkCollisionHorizontal(glm::vec3& collision_position, glm::vec3& collision_normal)
	{
		if (m_noclip)
			return false;

		for (int i = 0; i < exposed_blocks.size(); i += 1)
		{
			glm::vec3 block_collision_min = exposed_blocks[i];
			glm::vec3 block_collision_max = exposed_blocks[i] + glm::ivec3(1);

			glm::vec3 camera_collision_min = m_position + glm::vec3(-m_width, -m_width, -m_height);
			glm::vec3 camera_collision_max = m_position + glm::vec3(m_width, m_width, 0.0f);

			if ((camera_collision_min.x <= block_collision_max.x && camera_collision_max.x >= block_collision_min.x) &&
				(camera_collision_min.y <= block_collision_max.y && camera_collision_max.y >= block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				collision_position = camera_collision_min - block_collision_min;

				if (collision_position.z < abs(collision_position.x) || collision_position.z < abs(collision_position.y))
				{
					if (glm::abs(collision_position.x) > glm::abs(collision_position.y))
					{
						collision_normal = glm::normalize(glm::vec3(collision_position.x, 0.0f, 0.0f));
					}
					else
					{
						collision_normal = glm::normalize(glm::vec3(0.0f, collision_position.y, 0.0f));
					}

					std::cout << "horizontal collision: (" << i << "/" << exposed_blocks.size() << ")\n";
					return true;
				}
			}
		}

		return false;
	}

	std::tuple<glm::ivec3, glm::vec3> getRayIntersect()
	{
		for (float r = 0; r < m_ray_length; r += m_ray_step)
		{
			glm::vec3 ray = m_front * r;

			for (int i = 0; i < exposed_blocks.size(); i++)
			{
				glm::vec3 block_collision_min = exposed_blocks[i];
				glm::vec3 block_collision_max = exposed_blocks[i] + glm::ivec3(1);

				if ((m_position.x + ray.x <= block_collision_max.x && m_position.x + ray.x >= block_collision_min.x) &&
					(m_position.y + ray.y <= block_collision_max.y && m_position.y + ray.y >= block_collision_min.y) &&
					(m_position.z + ray.z <= block_collision_max.z && m_position.z + ray.z >= block_collision_min.z))
				{
					return std::make_tuple(exposed_blocks[i], m_position + ray - (glm::vec3(exposed_blocks[i]) + glm::vec3(0.5f)));
				}
			}
		}
		return std::make_tuple(glm::ivec3(-1), glm::vec3(-1.0f));
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	glm::mat4 getProjectionMatrix()
	{
		return glm::perspective(glm::radians(m_fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
	}

	void processKeyboard(int key)
	{
		if (m_noclip)
			m_front_move = m_front;
		else
			m_front_move = m_front_plane;

		glm::vec3 movement_vector = glm::vec3(0.0f);

		if (key == GLFW_KEY_W)
			movement_vector += m_front_move;
		if (key == GLFW_KEY_S)
			movement_vector -= m_front_move;
		if (key == GLFW_KEY_A)
			movement_vector -= m_right;
		if (key == GLFW_KEY_D)
			movement_vector += m_right;

		applyMovement(movement_vector);

		if (key == GLFW_KEY_SPACE)
			applyJump();
	}

	void processMouseMovement(float offset_x, float offset_y)
	{
		offset_x *= m_sensitivity;
		offset_y *= m_sensitivity;

		m_yaw += offset_x;
		m_pitch += offset_y;

		if (m_pitch > 89.99f)
			m_pitch = 89.99f;
		if (m_pitch < -89.99f)
			m_pitch = -89.99f;

		updateCameraVectors();
	}

	void processMouseScroll(float offset_y)
	{
		m_fov -= (float)offset_y * 3.0f;

		if (m_fov < 10.0f)
			m_fov = 10.0f;
		if (m_fov > 90.0f)
			m_fov = 90.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front{};
		front.x = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_pitch));
		m_front = glm::normalize(front);

		m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_up = glm::normalize(glm::cross(m_right, m_front));
		m_front_plane = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), m_right));
	}

	int divideInt(float a, float b) {
		if (a < 0) a -= 1;
		return (int)(a / b - ((int)a % (int)b < 0 ? 1 : 0));
	}
};