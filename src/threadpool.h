#pragma once

#include "global.h"

#include <thread>
#include <functional>
#include <deque>
#include <mutex>

class Threadpool
{
public:
	std::vector<std::thread> threads;
	std::deque<std::function<void()>> queue;

	std::mutex queue_mutex;
	std::mutex update_mutex;

	void createThreads(int n);
	void awaitJob();
};