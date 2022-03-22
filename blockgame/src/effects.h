#pragma once

#include <glm/glm.hpp>

class Light
{
public:
	glm::vec3 direction = glm::vec3(3.0f, -2.0f, 5.0f);;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ambient = glm::vec3(0.4f);
	glm::vec3 diffuse = glm::vec3(0.5f);
	glm::vec3 specular = glm::vec3(0.3f);

	void update();
};

class Fog
{
public:
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	float distance = 60.0f;
	float dropoff = 20.0f;
};