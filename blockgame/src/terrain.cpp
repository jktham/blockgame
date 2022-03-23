#include "terrain.h"
#include "global.h"

#include <glm/glm.hpp>

#include <vector>
#include <iostream>

float Terrain::getGroundHeight(int x, int y)
{
	float h = (float)CHUNK_SIZE.z / 2.0f;

	int offset = 0;
	float frequency = 1.5f;
	float amplitude = 3.0f;

	for (int i = 0; i < 4; i++)
	{
		h += getPerlin(x + offset, y + offset, frequency, amplitude);
		offset += 1000;
		frequency *= 0.5f;
		amplitude *= 2.0f;
	}

	//h = sin(x * 0.1f) * 10.0f + 32.0f;
	//h = (x * x + y * y) * 0.1f;
	//h = abs(x - y);
	return h;
}

float Terrain::interpolate(float a, float b, float w)
{
	//return (b - a) * w + a;
	//return (b - a) * (float)(3.0f * pow(w, 2) - 2.0f * pow(w, 3)) + a;
	return (b - a) * (float)(6.0f * pow(w, 5) - 15.0f * pow(w, 4) + 10.0f * pow(w, 3)) + a;
}

glm::vec2 Terrain::getRandomGridVector(int x, int y)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;

	unsigned a = x;
	unsigned b = y;

	a *= 3284157443;
	b ^= a << s | a >> (w - s);
	b *= 1911520717;
	a ^= b << s | b >> (w - s);
	a *= 2048419325;
	float r = a * (3.14159265f / ~(~0u >> 1));

	glm::vec2 v = glm::vec2(0.0f);
	v.x = cos(r);
	v.y = sin(r);

	return v;
}

float Terrain::getDotProduct(int x, int y, float fx, float fy)
{
	glm::vec2 g = getRandomGridVector(x, y);

	float dx = fx - (float)x;
	float dy = fy - (float)y;

	return (dx * g.x + dy * g.y);
}

float Terrain::getPerlin(int x, int y, float frequency, float amplitude)
{
	float fx = (float)x / (float)CHUNK_SIZE.x * frequency;
	float fy = (float)y / (float)CHUNK_SIZE.y * frequency;

	int x0 = (int)floor(fx);
	int x1 = x0 + 1;
	int y0 = (int)floor(fy);
	int y1 = y0 + 1;

	float wx = fx - (float)x0;
	float wy = fy - (float)y0;

	float n0, n1;
	float v0, v1;
	float v;

	n0 = getDotProduct(x0, y0, fx, fy);
	n1 = getDotProduct(x1, y0, fx, fy);
	v0 = interpolate(n0, n1, wx);

	n0 = getDotProduct(x0, y1, fx, fy);
	n1 = getDotProduct(x1, y1, fx, fy);
	v1 = interpolate(n0, n1, wx);

	v = interpolate(v0, v1, wy);

	return v * amplitude;
}