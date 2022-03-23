#pragma once

#include "glm/glm.hpp"

class Terrain
{
public:
	float getGroundHeight(int x, int y);

	float interpolate(float a, float b, float w);
	float getDotProduct(int x, int y, float fx, float fy);
	glm::vec2 getRandomGridVector(int x, int y);
	float getPerlin(int x, int y, float frequency, float amplitude);
};