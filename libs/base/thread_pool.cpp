//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "thread_pool.h"
#include "contract.h"

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

void thread_pool::queue_delayed( const std::function<void(void)> &f, double seconds )
{
	using namespace std::chrono;

	precondition( f, "invalid function to queue" );
	if ( seconds <= 0.0 )
		queue( f );

	duration<double> wait_time( seconds );

	auto queue_time = steady_clock::now() + duration_cast<steady_clock::duration>( wait_time );

	std::unique_lock<std::mutex> lock( _mutex );
//	if ( _delayed.empty() )
//		_delay_thread = std::thread( [=]( void ) { this->process_delay(); } );

	_delayed[queue_time] = f;
}

////////////////////////////////////////

void thread_pool::queue( const std::function<void(void)> &f )
{
	precondition( f, "invalid function to queue" );

	std::unique_lock<std::mutex> lock( _mutex );
	if ( _workers.empty() )
		return f();
	_work.push_back( f );
	_condition.notify_one();
}

////////////////////////////////////////

void thread_pool::work( void )
{
	while ( true )
	{
		std::function<void(void)> task;
		{
			std::unique_lock<std::mutex> lock( _mutex );
			while ( !_done && _work.empty() )
				_condition.wait( lock );

			if ( _done )
				break;

			task = std::move( _work.front() );
			_work.pop_front();
		}
		try
		{
			task();
		}
		catch ( ... )
		{
			// ignore all errors
		}
	}
}

////////////////////////////////////////

}


