#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <functional>

class Item
{
public:
	std::string name = "";
	int stacksize = 0;
	bool placeable = false;
	bool usable = false;
	bool interactable = false;
	std::function<void()> use = []() {};
	std::function<void()> interact = []() {};
};

class Stack
{
public:
	int id = 0;
	int amount = 0;
};

class Inventory
{
public:
	std::vector<Item> items = std::vector<Item>(15);
	std::vector<Stack> slots = std::vector<Stack>(10);
	int current_slot = 0;

	void initializeItems();
	void giveItem(int id, int amount);
	void takeItem(int id, int amount);
	void selectItem(int id);

	void dig(int size);
	void excavate(int limit);

	void openChest(glm::vec3 pos);
};