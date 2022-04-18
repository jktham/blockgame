#pragma once

#include <string>

class Console
{
public:
	std::string entry;

	void submit();
	void addLetter(int id);
	void removeLetter();
};