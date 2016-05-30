//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

static void initThreading( void )
{
	theThreadObj = std::make_shared<image::threading>( base::thread::core_count() );
	std::atexit( shutdownThreading );
}

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

threading::threading( int tCount )
	: _shutdown( false )
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

	int nT = static_cast<int>( _threads.size() );

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
threading::get( void )
{
	std::call_once( initThreadingFlag, initThreading );

	return *(theThreadObj);
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
			base::print_exception( std::cerr, e );
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



