#ifndef WORKER_THREAD_HPP
#define WORKER_THREAD_HPP

#include<concurrent_queue.h>
#include<functional>
#include<atomic>
#include<future>
#ifndef WORKER_THREAD_HPP
#define WORKER_THREAD_HPP

#include<concurrent_queue.h>
#include<functional>
#include<atomic>
#include<future>
#include<thread>
#include<bind>

class WorkerThread
{
private:
	std::atomic<bool> alive;
	concurrency::concurrent_queue<std::function<void()>> workitems;
	std::thread thethread;

	void work()
	{
		while (alive)
		{
			std::function<void()> func;
			if (workitems.try_pop())
			{
				func();
			}
			std::this_thread::yield();
		}
	}

public:
	WorkerThread() :alive(true), thread([this]() { this->work(); })
	{}
	template<class WIFunc>
	auto enqueue(WIFunc f)->std::future<decltype(f())>
	{
		std::package_task<decltype(f())> task(f);
		std::future<decltype(f())> result = task.get_future();
		std::function<void()> func=std::bind([](std::package_task<decltype(f())>& tk),std::move(task));
		workitems.emplace_back(std::move(func));
		return result;
	}
	~WorkerThread()
	{
		alive = false;
		thethread.join();
	}
};


#endif
