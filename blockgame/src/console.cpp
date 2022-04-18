#include "console.h"
#include "inventory.h"
#include "game.h"
#include "world.h"
#include "global.h"

#include <glm/glm.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

void Console::submit()
{
	entry.erase(entry.find_last_not_of(" ") + 1);
	std::stringstream ss(entry + " ");
	std::string item;
	std::vector<std::string> tokens;
	game->disableConsole();

	while (std::getline(ss, item, (char)32))
		tokens.push_back(item);

	if (tokens[0] == "GIVE" && tokens.size() == 2)
	{
		inventory->giveItem(std::stoi(tokens[1]), 1);
	}
	else if (tokens[0] == "GIVE" && tokens.size() == 3)
	{
		inventory->giveItem(std::stoi(tokens[1]), std::stoi(tokens[2]));
	}

	else if (tokens[0] == "TAKE" && tokens.size() == 2)
	{
		inventory->takeItem(std::stoi(tokens[1]), 1);
	}
	else if (tokens[0] == "TAKE" && tokens.size() == 3)
	{
		inventory->takeItem(std::stoi(tokens[1]), std::stoi(tokens[2]));
	}

	else if (tokens[0] == "PLACE" && tokens.size() == 5)
	{
		world->placeBlock(glm::vec3(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3])), std::stoi(tokens[4]));
		world->updateBlockChange(glm::vec3(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3])));
	}
	else if (tokens[0] == "BREAK" && tokens.size() == 4)
	{
		world->breakBlock(glm::vec3(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3])));
		world->updateBlockChange(glm::vec3(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3])));
	}
	else
		std::cout << "command (" << ss.str() << ") not recognized\n";
}

void Console::addLetter(int id)
{
	entry += (char)id;
}

void Console::removeLetter()
{
	if (entry.size() > 0)
		entry.pop_back();
}