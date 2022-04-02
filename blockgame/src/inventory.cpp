#include "inventory.h"
#include "player.h"

#include <string>
#include <vector>
#include <functional>

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
}

void Inventory::giveItem(int id, int amount)
{
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