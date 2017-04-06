//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



