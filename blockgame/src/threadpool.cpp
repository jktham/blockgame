#include "threadpool.h"
#include "global.h"

#include <thread>
#include <functional>
#include <deque>
#include <chrono>

void Threadpool::createThreads(int n)
{
	for (int i = 0; i < n; i++)
	{
		threads.push_back(std::thread([this] { this->awaitJob(); }));
	}
}

void Threadpool::awaitJob()
{
	while (true)
	{
		queue_mutex.lock();
		if (!queue.empty())
		{
			std::function job = queue.front();
			queue.pop_front();
			queue_mutex.unlock();
			job();
		}
		else
		{
			queue_mutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}