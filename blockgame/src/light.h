#pragma once

class Light
{
public:
	glm::vec3 m_direction = glm::vec3(3.0f, -2.0f, 5.0f);;
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_ambient = glm::vec3(0.4f);
	glm::vec3 m_diffuse = glm::vec3(0.5f);
	glm::vec3 m_specular = glm::vec3(0.3f);

	void update()
	{
		//m_direction = glm::vec3(50.0f, 50.0f + sin(current_frame) * 100.0f, cos(current_frame) * 50.0f);
	}
};