
#include "thread_pool.h"

namespace base
{

////////////////////////////////////////

thread_pool::thread_pool( size_t threads )
{
	for ( size_t i = 0; i < threads; ++i )
		_workers.emplace_back( [=]() { this->work(); } );
}

////////////////////////////////////////

thread_pool::~thread_pool( void )
{
	{
		std::unique_lock<std::mutex> lock( _mutex );
		_done = true;
		_condition.notify_all();
	}

	for ( auto &t: _workers )
		t.join();
}

////////////////////////////////////////

void thread_pool::queue( const std::function<void(void)> &f )
{
	std::unique_lock<std::mutex> lock( _mutex );
	if ( _workers.empty() )
		return f();
	_work.push_back( f );
	_condition.notify_one();
}

////////////////////////////////////////

void thread_pool::work( void )
{
	std::function<void(void)> task;
	while ( true )
	{
		{
			std::unique_lock<std::mutex> lock( _mutex );
			while ( !_done && _work.empty() )
				_condition.wait( lock );

			if ( _done )
				break;

			task = std::move( _work.front() );
			_work.pop_front();
		}
		task();
	}
}

////////////////////////////////////////

}


