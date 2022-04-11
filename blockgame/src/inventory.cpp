#include "inventory.h"
#include "player.h"
#include "world.h"
#include "camera.h"

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>

void Inventory::initializeItems()
{
	items[0].name = "";
	items[0].stacksize = 0;
	items[0].placeable = false;
	items[0].usable = false;
	items[0].use = []() {};

	items[1].name = "Grass";
	items[1].stacksize = 64;
	items[1].placeable = true;
	items[1].usable = false;
	items[1].use = []() {};

	items[2].name = "Dirt";
	items[2].stacksize = 64;
	items[2].placeable = true;
	items[2].usable = false;
	items[2].use = []() {};

	items[3].name = "Stone";
	items[3].stacksize = 64;
	items[3].placeable = true;
	items[3].usable = false;
	items[3].use = []() {};

	items[4].name = "Bedrock";
	items[4].stacksize = 64;
	items[4].placeable = true;
	items[4].usable = false;
	items[4].use = []() {};

	items[5].name = "Cobblestone";
	items[5].stacksize = 64;
	items[5].placeable = true;
	items[5].usable = false;
	items[5].use = []() {};

	items[6].name = "Log";
	items[6].stacksize = 64;
	items[6].placeable = true;
	items[6].usable = false;
	items[6].use = []() {};

	items[7].name = "Leaves";
	items[7].stacksize = 64;
	items[7].placeable = true;
	items[7].usable = false;
	items[7].use = []() {};

	items[8].name = "Water";
	items[8].stacksize = 64;
	items[8].placeable = true;
	items[8].usable = false;
	items[8].use = []() {};

	items[9].name = "Dig Tool";
	items[9].stacksize = 1;
	items[9].placeable = false;
	items[9].usable = true;
	items[9].use = [this]() {
		dig(2);
	};

	items[10].name = "Excavate Tool";
	items[10].stacksize = 1;
	items[10].placeable = false;
	items[10].usable = true;
	items[10].use = [this]() {
		excavate(64);
	};

	items[11].name = "Iron Ore";
	items[11].stacksize = 64;
	items[11].placeable = true;
	items[11].usable = false;
	items[11].use = []() {};

	items[12].name = "Gold Ore";
	items[12].stacksize = 64;
	items[12].placeable = true;
	items[12].usable = false;
	items[12].use = []() {};

	items[13].name = "Diamond Ore";
	items[13].stacksize = 64;
	items[13].placeable = true;
	items[13].usable = false;
	items[13].use = []() {};
}

void Inventory::giveItem(int id, int amount)
{
	if (id == 0)
		return;

	for (int i = 0; i < slots.size(); i++)
	{
		if (slots[i].id == id && slots[i].amount < items[slots[i].id].stacksize)
		{
			slots[i].id = id;
			slots[i].amount += amount;
			if (slots[i].amount > items[slots[i].id].stacksize)
			{
				int overflow = slots[i].amount - items[slots[i].id].stacksize;
				slots[i].amount = items[slots[i].id].stacksize;
				giveItem(id, overflow);
			}
			return;
		}
	}
	for (int i = 0; i < slots.size(); i++)
	{
		if (slots[i].id == 0)
		{
			slots[i].id = id;
			slots[i].amount = amount;
			if (slots[i].amount > items[slots[i].id].stacksize)
			{
				int overflow = slots[i].amount - items[slots[i].id].stacksize;
				slots[i].amount = items[slots[i].id].stacksize;
				giveItem(id, overflow);
			}
			return;
		}
	}
}

void Inventory::removeItem(int id)
{
	if (slots[current_slot].id == id)
	{
		slots[current_slot].amount -= 1;
		if (slots[current_slot].amount <= 0)
		{
			slots[current_slot].id = 0;
		}
	}
	else
	{
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i].id == id)
			{
				slots[i].amount -= 1;
				if (slots[i].amount <= 0)
				{
					slots[i].id = 0;
				}
				return;
			}
		}
	}
}

void Inventory::selectItem(int id)
{
	if (id != 0)
	{
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i].id == id)
			{
				current_slot = i;
				return;
			}
		}
	}
}

void Inventory::dig(int size)
{
	glm::vec3 pos = glm::floor(camera->getRayIntersect());
	if (pos == glm::vec3(-1.0f))
		return;

	for (int i = -size; i <= size; i++)
		for (int j = -size; j <= size; j++)
			for (int k = -size; k <= size; k++)
			{
				world->breakBlock(pos + glm::vec3(i, j, k));
			}

	world->updateBlockChange(pos);
}

void Inventory::excavate(int limit)
{
	glm::vec3 pos = glm::floor(camera->getRayIntersect());
	if (pos == glm::vec3(-1.0f))
		return;

	int type = world->getBlockType(pos);
	std::vector<glm::vec3> found;
	std::deque<glm::vec3> queue;
	queue.push_back(pos);
	int iterations = 0;

	while (queue.size() > 0 && found.size() < limit && iterations < 1000)
	{
		iterations += 1;
		glm::vec3 new_pos = queue.front();
		queue.pop_front();

		if (world->getBlockType(new_pos) == type)
		{
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
					for (int k = -1; k <= 1; k++)
					{
						glm::vec3 next_pos = new_pos + glm::vec3(i, j, k);
						if (std::count(queue.begin(), queue.end(), next_pos) == 0 && std::count(found.begin(), found.end(), next_pos) == 0 && !(i == 0 && j == 0 && k == 0))
							queue.push_back(next_pos);
					}

			if (std::count(found.begin(), found.end(), new_pos) == 0)
				found.push_back(new_pos);
		}
	}

	for (int i = 0; i < found.size(); i++)
		world->breakBlock(found[i]);

	world->updateBlockChange(pos);
}