﻿#pragma once

class Light
{
public:
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);

	void update(float time)
	{
		m_position = glm::vec3(time * 5.0f, 0.0f, 0.0f);
	}
};