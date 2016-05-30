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

	/// calls function f on the range split by the number of threads
	/// live, and does not return until they have all finished.
	void dispatch( const std::function<void(int, int)> &f, int start, int N );

	inline void dispatch( const std::function<void(int, int)> &f, const plane &p )
	{
		dispatch( f, 0, p.height() );
	}

	/// Shutdown the threads
	void shutdown( void );

	/// Get the singleton threading object
	static threading &get( void );
private:
	void bee( void );

	std::mutex _mutex;
	std::condition_variable _cond;
	std::condition_variable _wait_cond;
	std::vector<std::thread> _threads;
	struct func_to_apply
	{
		std::function<void(int, int)> f;
		int start;
		int end;
		std::atomic<int> *counter;
	};
	std::deque<func_to_apply> _funcs;
	std::atomic<bool> _shutdown;
};

} // namespace image



