#pragma once

#include <string>

enum class State
{
	MENU,
	ALIVE,
};

class Game
{
public:
	State state = State::MENU;

	void start();
	void quit();

	void save(std::string path);
	void load(std::string path);
};