//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>

////////////////////////////////////////

namespace base
{

///
/// @brief Class wait provides an abstraction around a handle
/// to be waited upon.
///
/// NB: right now this assumes you are waiting to read
///
class wait
{
public:
#ifdef _WIN32
	using wait_type = LONG_PTR;
	using poll_type = HANDLE;
	static const wait_type INVALID_WAIT = LONG_PTR(-1);
#else
	using wait_type = int;
	using poll_type = int;
	static const wait_type INVALID_WAIT = -1;
#endif
	wait( void ) = default;
	wait( wait_type w ) : _w( w ) {}
	wait( const wait & ) = default;
	wait( wait && ) = default;
	wait &operator=( const wait & ) = default;
	wait &operator=( wait && ) = default;
	~wait( void ) = default;

	bool is_valid( void ) const { return _w != INVALID_WAIT; }
	poll_type waitable( void ) const { return (poll_type)_w; }

private:
	wait_type _w = INVALID_WAIT;
};

void wait_for( const wait &w );

wait wait_for_any( const std::vector<wait> &w );
template <typename ...Args>
wait wait_for_any( const wait &w, Args &&... ew )
{
	return wait_for_any( std::vector<wait>{ w, ew... } );
}
						 
} // namespace base



