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
	: _count( tCount ), _shutdown( false )
{
	size_t n = 0;
	if ( tCount > 0 )
	{
		n = static_cast<size_t>( tCount );
		std::cout << "Starting " << n << " threads for image processing..." << std::endl;
//		std::lock_guard<std::mutex> lk( std::mutex );
		_threads.resize( n );
		_dispatch.reset( new dispatch_logic[n] );
		for ( size_t i = 0; i != n; ++i )
		{
			_dispatch[i].f = nullptr;
			_dispatch[i].start = 0;
			_dispatch[i].end = 0;
			_dispatch[i].dispatch_mode.store( kDispatchEmpty );
		}

		for ( size_t i = 0; i != n; ++i )
			_threads[i] = std::thread( &threading::bee, this, i );
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

	// TODO: do we want to oversubscribe a bit, or only dispatch n-1
	// threads?  right now, it's simpler to oversubscribe, although we
	// could adjust count if that turns to be an issue
	
//	int nPer = ( N + ( nT - 1 ) ) / nT;
	int nPer = ( N + _count ) / ( _count + 1 );
	int curS = start;
	int endV = start + N;

	// TODO: change this to enable other things to be in flight
	// when this comes in, allowing us to do recursive evaluation...
	// at that point, this code should call participate...
	size_t curIdx = 0;
	while ( ( curS + nPer ) < endV )
	{
		int chunkE = curS + nPer;
		auto &disp = _dispatch[curIdx];
		disp.start = curS;
		disp.end = chunkE;
		disp.f = &f;
		disp.dispatch_mode.store( kDispatchReady );
		_thread_sema.signal();
		curS = chunkE;
		++curIdx;
	}
	if ( curS < endV )
		f( curIdx, curS, endV );

	while ( curIdx > 0 )
	{
		_wait_sema.wait();
		--curIdx;
	}
}

////////////////////////////////////////

bool
threading::participate( size_t tIdx )
{
	bool check = true;
	while ( check )
	{
		check = false;
		for ( size_t j = 0, N = static_cast<size_t>( _count ); j < N; ++j )
		{
			auto &x = _dispatch[j];
			int s = x.dispatch_mode.load( std::memory_order_relaxed );
			if ( s == kDispatchReady )
			{
				check = true;
				int ns = kDispatchInProcess;
				if ( x.dispatch_mode.compare_exchange_weak( s, ns, std::memory_order_release, std::memory_order_relaxed ) )
				{
					(*(x.f))( tIdx, x.start, x.end );
					ns = kDispatchEmpty;
					x.dispatch_mode.compare_exchange_strong( s, ns );
					_wait_sema.signal();
					return true;
				}
			}
		}
	}
	return false;
}

////////////////////////////////////////

void
threading::shutdown( void )
{
	_shutdown = true;
	_thread_sema.signal( _count );
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
	while ( true )
	{
		if ( _shutdown )
			break;

		if ( participate( tIdx ) )
			continue;

		_thread_sema.wait();
	}
}

////////////////////////////////////////

} // image



