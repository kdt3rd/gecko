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
#include <base/semaphore.h>
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


#if __cplusplus > 201402L
	static constexpr size_t kAlign = std::hardware_destructive_interference_size;
#else
	static constexpr size_t kAlign = 64;
#endif
	bool participate( size_t tIdx );
	static const int kDispatchEmpty = 0;
	static const int kDispatchReady = 1;
	static const int kDispatchInProcess = 2;
	struct dispatch_logic
	{
		int start;
		int end;
		const std::function<void(size_t, int, int)> *f;

		/// if the dispatch mode is 0, function is invalid / nothing to do
		/// if the dispatch mode is 1, ready for processing
		/// if the dispatch mode is 2, in process
		std::atomic<int> dispatch_mode;

		// prevent false sharing...
		char padding[kAlign - sizeof(int)*3+sizeof(void *)];
	};

	int _count = 0;
	std::unique_ptr<dispatch_logic []> _dispatch;
	std::vector<std::thread> _threads;
	alignas(kAlign) base::semaphore _thread_sema;
	alignas(kAlign) base::semaphore _wait_sema;
	alignas(kAlign) std::atomic<bool> _shutdown;
};

} // namespace image



