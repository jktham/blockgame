#include "terrain.h"
#include "perlin.h"
#include "global.h"

#include <glm/glm.hpp>

#include <vector>

double Terrain::getGroundHeight(int x, int y)
{
	double h = perlin.octave2D_01(x * 0.05f, y * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE.z / 2.0f;
	return h;
}