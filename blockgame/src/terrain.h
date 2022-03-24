#pragma once

#include "glm/glm.hpp"

class Terrain
{
public:
	int OFFSET = 0;
	float FREQUENCY = 0.18f; // 1.0f -> 0 at chunk corners
	float AMPLITUDE = 24.0f;

	int OCTAVES = 4;
	int SHIFT = 1000; // offset summand
	float LACUNARITY = 2.0f; // frequency factor
	float PERSISTENCE = 0.5f; // amplitude factor

	float getGroundHeight(int x, int y);

	float interpolate(float a, float b, float w);
	float getDotProduct(int x, int y, float fx, float fy);
	glm::vec2 getRandomGridVector(int x, int y);
	float getPerlin(int x, int y, float frequency, float amplitude);
};