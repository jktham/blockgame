#pragma once

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
};