#pragma once

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

	Camera()
	{
		updateCameraVectors();
	}

	void reset()
	{
		position = glm::vec3(0.0f, 0.0f, CHUNK_SIZE.z / 2.0f + 16.0f);
		yaw = 45.0f;
		pitch = 0.0f;
		vertical_velocity = 0.0f;
		updateCameraVectors();
	}

	void applyGravity()
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
			position.z = collision_blocks_v[0].z + 1.0f + height;
		}

		position += glm::vec3(0.0f, 0.0f, 1.0f) * vertical_velocity * delta_time;

		if (vertical_velocity <= -50.0f)
			vertical_velocity = -50.0f;
		if (position.z < -32.0f)
			position.z = CHUNK_SIZE.z + 64.0f;
	}

	void applyMovement(glm::vec3 direction)
	{
		if (!detectCollisionH())
		{
			float velocity = speed * delta_time;
			position += direction * velocity;
		}
		else
		{
			for (int i = 0; i < collision_blocks_h.size(); i++) // TODO: consider multiblock collisions
			{
				glm::vec3 collision_offset = position + glm::vec3(-width, -width, -height) - collision_blocks_h[i];
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

	void applyJump()
	{
		if (detectCollisionV())
		{
			vertical_velocity = 7.5f;
			position.z += 0.01f;
		}
	}

	bool detectCollisionV()
	{
		collision_blocks_v = {};

		if (noclip)
			return false;

		for (int i = 0; i < exposed_blocks.size(); i += 1)
		{
			glm::vec3 block_collision_min = exposed_blocks[i];
			glm::vec3 block_collision_max = exposed_blocks[i] + glm::vec3(1.0f);

			glm::vec3 camera_collision_min = position + glm::vec3(-width, -width, -height);
			glm::vec3 camera_collision_max = position + glm::vec3(width, width, 0.0);

			if ((camera_collision_min.x < block_collision_max.x && camera_collision_max.x > block_collision_min.x) &&
				(camera_collision_min.y < block_collision_max.y && camera_collision_max.y > block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				glm::vec3 collision_offset = camera_collision_min - block_collision_min;

				if (collision_offset.z > abs(collision_offset.x) && collision_offset.z > abs(collision_offset.y))
				{
					collision_blocks_v.push_back(exposed_blocks[i]);
					std::cout << "vertical collision: (" << i << "/" << exposed_blocks.size() << ")\n";
				}
			}
		}

		if (!collision_blocks_v.empty())
		{
			return true;
		}
		return false;
	}

	bool detectCollisionH()
	{
		collision_blocks_h = {};

		if (noclip)
			return false;

		for (int i = 0; i < exposed_blocks.size(); i += 1)
		{
			glm::vec3 block_collision_min = exposed_blocks[i];
			glm::vec3 block_collision_max = exposed_blocks[i] + glm::vec3(1.0f);

			glm::vec3 camera_collision_min = position + glm::vec3(-width, -width, -height);
			glm::vec3 camera_collision_max = position + glm::vec3(width, width, 0.0f);

			if ((camera_collision_min.x <= block_collision_max.x && camera_collision_max.x >= block_collision_min.x) &&
				(camera_collision_min.y <= block_collision_max.y && camera_collision_max.y >= block_collision_min.y) &&
				(camera_collision_min.z <= block_collision_max.z && camera_collision_max.z >= block_collision_min.z))
			{
				glm::vec3 collision_offset = camera_collision_min - block_collision_min;

				if (collision_offset.z < abs(collision_offset.x) || collision_offset.z < abs(collision_offset.y))
				{
					collision_blocks_h.push_back(exposed_blocks[i]);
					std::cout << "horizontal collision: (" << i << "/" << exposed_blocks.size() << ")\n";
				}
			}
		}

		if (!collision_blocks_h.empty())
		{
			return true;
		}
		return false;
	}

	std::tuple<glm::vec3, glm::vec3> getRayIntersect()
	{
		reachable_blocks = {};
		for (int i = 0; i < exposed_blocks.size(); i += 1)
		{
			if (glm::length(position - exposed_blocks[i]) < ray_length + 1.0f)
			{
				reachable_blocks.push_back(exposed_blocks[i]);
			}
		}

		for (float r = 0; r < ray_length; r += ray_step)
		{
			glm::vec3 ray = front * r;

			for (int i = 0; i < reachable_blocks.size(); i++)
			{
				glm::vec3 block_collision_min = reachable_blocks[i];
				glm::vec3 block_collision_max = reachable_blocks[i] + glm::vec3(1.0f);

				if ((position.x + ray.x <= block_collision_max.x && position.x + ray.x >= block_collision_min.x) &&
					(position.y + ray.y <= block_collision_max.y && position.y + ray.y >= block_collision_min.y) &&
					(position.z + ray.z <= block_collision_max.z && position.z + ray.z >= block_collision_min.z))
				{
					return std::make_tuple(reachable_blocks[i], position + ray - (glm::vec3(reachable_blocks[i]) + glm::vec3(0.5f)));
				}
			}
		}
		return std::make_tuple(glm::vec3(-1.0f), glm::vec3(-1.0f));
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	glm::mat4 getProjectionMatrix()
	{
		return glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
	}

	void processKeyboard(int key)
	{
		if (noclip)
			front_move = front;
		else
			front_move = front_plane;

		glm::vec3 movement_vector = glm::vec3(0.0f);

		if (key == GLFW_KEY_W)
			movement_vector += front_move;
		if (key == GLFW_KEY_S)
			movement_vector -= front_move;
		if (key == GLFW_KEY_A)
			movement_vector -= right;
		if (key == GLFW_KEY_D)
			movement_vector += right;

		applyMovement(movement_vector);

		if (key == GLFW_KEY_SPACE)
			applyJump();
	}

	void processMouseMovement(float offset_x, float offset_y)
	{
		offset_x *= sensitivity;
		offset_y *= sensitivity;

		yaw += offset_x;
		pitch += offset_y;

		if (pitch > 89.9999f)
			pitch = 89.9999f;
		if (pitch < -89.9999f)
			pitch = -89.9999f;

		updateCameraVectors();
	}

private:
	void updateCameraVectors()
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

	int divideInt(float a, float b)
	{
		if (a < 0) a -= 1;
		return (int)(a / b - ((int)a % (int)b < 0 ? 1 : 0));
	}
};