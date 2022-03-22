#pragma once

#include "global.h"

#include <thread>
#include <functional>
#include <deque>

class Threadpool
{
public:
	std::vector<std::thread> threads;
	std::deque<std::function<void()>> queue;

	void createThreads(int n);
	void awaitJob();
};