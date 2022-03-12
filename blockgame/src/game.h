#pragma once

class Game
{
public:
	int state = 0;

	void start()
	{
		state = 1;
		first_mouse = true;

		world = new World;
		world->createChunks();
		world->generateChunkMesh();
		world->generateWorldMesh();
		world->updateVAO();

		camera = new Camera;
		current_chunk = glm::vec2(0.0f, 0.0f);
		last_chunk = glm::vec2(0.0f, 0.0f);
	}

	void quit()
	{
		state = 0;
		current_type = MAX_TYPE;
		last_type = -1;

		delete world;
		delete camera;
	}
};