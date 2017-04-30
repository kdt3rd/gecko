//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <deque>
#include <vector>
#include <functional>
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

	inline size_t size( void ) const { std::lock_guard<std::mutex> lk( _mutex ); return _threads.empty() ? size_t(1) : _threads.size(); }

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

	mutable std::mutex _mutex;
	std::condition_variable _cond;
	std::condition_variable _wait_cond;
	std::vector<std::thread> _threads;
	struct func_to_apply
	{
		std::function<void(size_t, int, int)> f;
		int start;
		int end;
		std::atomic<int> *counter;
	};
	std::deque<func_to_apply> _funcs;
	std::atomic<bool> _shutdown;
	std::atomic<int> _count;
};

} // namespace image



