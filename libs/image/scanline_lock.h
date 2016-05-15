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

#include <atomic>
#include <mutex>

////////////////////////////////////////

namespace dag
{

///
/// @brief Class scanline_lock provides a lightweight spin lock
///
/// This is done to also enable while waiting for a lock, processing
/// other graph requests.
///
class scanline_lock
{
public:
	inline scanline_lock( graph &g ) : _g( g ) {}
	inline ~scanline_lock( void ) {}

	inline void lock( void )
	{
		while ( _f.test_and_set( std::memory_order_acquire ) )
			_g.process_other_or_yield();
	}

	inline void unlock( void )
	{
		_f.clear( std::memory_order_acquire );
	}

private:
	scanline_lock( const scanline_lock & ) = delete;
	scanline_lock &operator=( const scanline_lock & ) = delete;
	graph &_g;
	std::atomic_flag _f = ATOMIC_FLAG_INIT;
};

} // namespace image



