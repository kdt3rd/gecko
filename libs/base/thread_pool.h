
#pragma once

#include <deque>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <map>

namespace base
{

////////////////////////////////////////

class thread_pool
{
public:
	thread_pool( size_t threads );
	~thread_pool( void );

	void queue_delayed( const std::function<void(void)> &f, double seconds );
	void queue( const std::function<void(void)> &f );

private:
	void work( void );
	void process_delay( void );

	bool _done = false;
	std::vector<std::thread> _workers;
	std::mutex _mutex;
	std::condition_variable _condition;
	std::deque<std::function<void(void)>> _work;

	std::thread _delay_thread;
	std::map<std::chrono::steady_clock::time_point,std::function<void(void)>> _delayed;
};

////////////////////////////////////////

}

