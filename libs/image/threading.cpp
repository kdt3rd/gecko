//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "threading.h"
#include <base/thread_util.h>
#include <base/contract.h>
#include <atomic>

////////////////////////////////////////

namespace
{

static std::shared_ptr<image::threading> theThreadObj;
std::once_flag initThreadingFlag;

static void shutdownThreading( void )
{
	if ( theThreadObj )
	{
		theThreadObj->shutdown();
		theThreadObj.reset();
	}
}

static void initThreading( int count )
{
	if ( count >= 0 )
		theThreadObj = std::make_shared<image::threading>( count );
	else
		theThreadObj = std::make_shared<image::threading>( base::thread::core_count() );
	std::atexit( shutdownThreading );
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

threading::threading( int tCount )
	: _shutdown( false ), _count( tCount )
{
	size_t n = 0;
	if ( tCount > 0 )
	{
		n = static_cast<size_t>( tCount );
		std::cout << "Starting " << n << " threads for image processing..." << std::endl;
		std::lock_guard<std::mutex> lk( std::mutex );
		_threads.resize( n );
		for ( size_t i = 0; i != n; ++i )
		{
			_threads[i] = std::thread( &threading::bee, this, i );
		}
	}
}

////////////////////////////////////////

threading::~threading( void )
{
}

////////////////////////////////////////

void
threading::dispatch( const std::function<void(size_t, int, int)> &f, int start, int N )
{
	precondition( N > 0, "attempt to dispatch with no items ({0}) to process", N );

	int nT = _count.load();
	if ( nT == 0 )
	{
		f( 0, start, start + N );
		return;
	}

	int nPer = ( N + ( nT - 1 ) ) / nT;
	int curS = start;
	int endV = start + N;
	std::atomic<int> funcCount( 0 );
	std::unique_lock<std::mutex> lk( _mutex );
	while ( curS < endV )
	{
		int curE = curS + nPer;
		if ( curE > endV )
			curE = endV;
		func_to_apply fa;
		fa.f = f;
		fa.start = curS;
		fa.end = curE;
		fa.counter = &funcCount;
		_funcs.emplace_back( fa );

		funcCount.fetch_add( 1, std::memory_order_relaxed );
		curS = curE;
	}

	_cond.notify_all();
	while ( funcCount.load() != 0 )
		_wait_cond.wait( lk );
}

////////////////////////////////////////

void
threading::shutdown( void )
{
	_shutdown = true;
	_cond.notify_all();
	for ( auto &t: _threads )
		t.join();
}

////////////////////////////////////////

threading &
threading::get( int count )
{
	std::call_once( initThreadingFlag, initThreading, count );

	return *(theThreadObj);
}

////////////////////////////////////////

void
threading::init( int count )
{
	std::call_once( initThreadingFlag, initThreading, count );
}

////////////////////////////////////////

void
threading::bee( size_t tIdx )
{
	std::unique_lock<std::mutex> lk( _mutex );
	while ( true )
	{
		while ( _funcs.empty() )
		{
			if ( _shutdown )
				return;

			_cond.wait( lk );
		}

		func_to_apply fa = _funcs.front();
		_funcs.pop_front();

		lk.unlock();
		try
		{
			fa.f( tIdx, fa.start, fa.end );
		}
		catch ( std::exception &e )
		{
			lk.lock();
			base::print_exception( std::cerr, e );
			lk.unlock();
		}

		if ( fa.counter->fetch_sub( 1, std::memory_order_release ) == 1 )
		{
			std::atomic_thread_fence( std::memory_order_acquire );
			_wait_cond.notify_all();
		}
		lk.lock();
	}
}

////////////////////////////////////////

} // image



