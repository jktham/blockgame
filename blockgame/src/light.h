#pragma once

class Light
{
public:
	glm::vec3 m_position = glm::vec3(-100.0f, 300.0f, 200.0f);;
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_ambient = glm::vec3(0.4f);
	glm::vec3 m_diffuse = glm::vec3(0.5f);
	glm::vec3 m_specular = glm::vec3(0.3f);

	void update(float time)
	{
		m_position = glm::vec3(50.0f + sin(time) * 100.0f, cos(time) * 50.0f, 50.0f);
	}
};