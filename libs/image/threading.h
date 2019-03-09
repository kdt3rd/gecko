// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <deque>
#include <vector>
#include <functional>
#include <base/semaphore.h>
//#include <base/lock_free_list.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "plane.h"
#include "image.h"

////////////////////////////////////////

namespace image
{

/// @brief custom thread pool specific for splitting and processing images.
///
/// we provide this as a singleton that is initialized at the first
/// retrieval and shutdown automatically.
///
/// It is specialized for handling functions that take a range of
/// scanlines in a plane.
///
/// The number of threads created is driven off the global number
/// returned by core_count in base/thread_util.h
///
class threading
{
public:
	threading( int nThreads );
	~threading( void );

	inline size_t size( void ) const { return static_cast<size_t>( _count + 1 ); }

	/// calls function f on the range split by the number of threads
	/// live, and does not return until they have all finished.
	void dispatch( const std::function<void(size_t, int, int)> &f, int start, int N );

	inline void dispatch( const std::function<void(size_t, int, int)> &f, const plane &p )
	{
		dispatch( f, p.y1(), p.height() );
	}

	/// Shutdown the threads
	void shutdown( void );

	/// Get the singleton threading object
	static threading &get( int count = -1 );
	static void init( int count = -1 );
private:
	void bee( size_t i );

	struct worker_bee
	{
		worker_bee( void );
		worker_bee( std::thread::id selfthread );

		std::atomic<worker_bee *> _next;

		size_t _index = 0;
		int _start = 0;
		int _end = 0;
		const std::function<void(size_t, int, int)> *_func = nullptr;
		std::atomic<bool> _finished;

		std::thread _thread;
		base::semaphore _sema;
		std::atomic<bool> _shutdown;
	private:
		void run_bee( void );
	};

	inline worker_bee *try_steal( void )
	{
		worker_bee *cur = _avail_workers.load( std::memory_order_relaxed );
		worker_bee *ret = nullptr;
		while ( cur )
		{
            ret = cur;
			// this avoids the ABA problem since we're always taking
			// the whole list and assigning in nullptr, so there's no
			// real 'B' to even start to have an ABA issue - if
			// someone puts the same 'A' back on the list, that's
			// fine, we're happy to use it since we're not deleting
			// these or anything...
			if ( _avail_workers.compare_exchange_weak( cur, nullptr, std::memory_order_release, std::memory_order_acquire ) )
                break;
		}
		return ret;
	}

	inline void put_back( worker_bee *wb )
	{
		worker_bee *endcur = wb;
		while ( endcur->_next.load( std::memory_order_relaxed ) )
			endcur = endcur->_next.load( std::memory_order_relaxed );

		worker_bee *curhead = _avail_workers.load( std::memory_order_relaxed );
		while ( true )
		{
			endcur->_next = curhead;
			if ( _avail_workers.compare_exchange_weak( curhead, wb, std::memory_order_release, std::memory_order_acquire ) )
                break;
		}
	}

	void donate( worker_bee &wb );

	// we don't care so much about the ABA problem since we aren't
	// deleting the items... and we want to checkout all the threads
	// at once if we can so we are always assigning in a nullptr and
	// taking or returning a chunk of a list
	std::atomic<worker_bee *> _avail_workers;
	//base::lock_free_list<worker_bee> _avail_workers;
	std::vector< std::unique_ptr<worker_bee> > _threads;
	int _count = 0;
};

} // namespace image



