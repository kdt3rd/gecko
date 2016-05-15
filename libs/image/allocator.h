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

#include <memory>
#include <algorithm>
#include <mutex>
#include <ostream>
#include <atomic>
#include <list>

////////////////////////////////////////

namespace image
{

/// @brief allocator provides a means to track memory usage
///
class allocator
{
public:
	static constexpr int floatAlignCount = 16;
	static constexpr size_t defaultAlign = 64;
	allocator( void );
	~allocator( void );

	/// Sets how much memory to keep around speculatively
	void set_stash_size( size_t maxBytes );

	/// Sets the allocation size at which point memory starts to cache
	/// Setting the cache limit to 0 will disable cache (BEWARE, you
	/// might swap)
	void set_cache_limit( size_t cacheStart );

	/// controls how many times an item will be skipped in the stash
	/// prior to being discarded as stale
	void set_skippiness( size_t s );

	/// allocates a generic 1D buffer scanline with specified byte alignment
	std::shared_ptr<void> allocate( size_t bytes, size_t align = defaultAlign );

	/// allocates a scanline float buffer, stretching the width to
	/// support AVX512 (stride) (well, whatever defaultAlign is set
	/// to)
	std::shared_ptr<float> scanline( int &stride, int w );
	/// allocates a 2D float buffer, stretching the width to support
	/// AVX512 (stride) (well, whatever defaultAlign is set to)
	std::shared_ptr<float> buffer( int &stride, int w, int h );

	void clear_stash( void ) noexcept;

	void report( std::ostream &os );
	static allocator &get( void );
private:
	void reduce_stash( size_t targsize );
	void *create( std::unique_lock<std::mutex> &lk, size_t bytes, size_t align, size_t &maxEntry );
	void destroy( void *p, size_t b );

	void return_misc( void * ) noexcept;
	void return_scan( float * ) noexcept;
	void return_buffer( float * ) noexcept;

	std::mutex _mutex;
	size_t _max_alloced = 0;
	size_t _max_buffers_live = 0;
	size_t _max_memory_live = 0;
	size_t _max_misc_size = 0;
	size_t _max_scan_size = 0;
	size_t _max_buffer_size = 0;

	size_t _cur_buffers_live = 0;
	size_t _cur_memory_live = 0;
	size_t _cur_alloced = 0;
	size_t _cur_stash_size = 0;

	size_t _max_stash_size = 0;
	size_t _stash_skippiness = 5;
	size_t _cache_start = 0;

	struct memMisc
	{
		void *ptr;
		size_t size;
		size_t skip_count;
	};
	std::list<memMisc> _inflight_misc;
	std::list<memMisc> _stash_misc;

	struct memScan
	{
		float *ptr;
		int w;
		int stride;
		size_t size;
		size_t skip_count;
	};
	std::list<memScan> _inflight_scan;
	std::list<memScan> _stash_scan;

	struct memBuf
	{
		float *ptr;
		int w;
		int h;
		int stride;
		size_t size;
		size_t skip_count;
	};
	std::list<memBuf> _inflight_buf;
	std::list<memBuf> _stash_buf;
};

} // namespace image



