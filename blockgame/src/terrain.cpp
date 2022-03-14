#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#include "global.h"
#include "perlin.h"
#include "terrain.h"

double Terrain::getGroundHeight(int x, int y)
{
	double h = perlin.octave2D_01(x * 0.05f, y * 0.05f, 4) * 10.0f + (float)CHUNK_SIZE.z / 2.0f;
	return h;
}