#pragma once

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
	std::function<void()> use = []() {};
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
	std::vector<Item> items = std::vector<Item>(9);
	std::vector<Stack> slots = std::vector<Stack>(10);
	int current_slot = 0;

	void initializeItems();
	void giveItem(int id, int amount);
	void removeItem(int id);
	void selectItem(int id);
};