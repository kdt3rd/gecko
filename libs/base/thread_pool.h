
#pragma once

#include <deque>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace base
{

////////////////////////////////////////

class thread_pool
{
public:
	thread_pool( size_t threads );
	~thread_pool( void );

	void queue( const std::function<void(void)> &f );

private:
	void work( void );

	bool _done = false;
	std::vector<std::thread> _workers;
	std::mutex _mutex;
	std::condition_variable _condition;
	std::deque<std::function<void(void)>> _work;
};

////////////////////////////////////////

}

