// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstdint>
#include <cstddef>
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
    using wait_type = intptr_t;
    using poll_type = void *;
#else
    using wait_type = int;
    using poll_type = int;
#endif
    static constexpr wait_type INVALID_WAIT = wait_type( -1 );

    wait( void ) = default;
    wait( wait_type w ) : _w( w ) {}
    wait( const wait & ) = default;
    wait( wait && )      = default;
    wait &operator=( const wait & ) = default;
    wait &operator=( wait && ) = default;
    ~wait( void )              = default;

    bool      is_valid( void ) const { return _w != INVALID_WAIT; }
    poll_type waitable( void ) const
    {
        return reinterpret_cast<poll_type>( _w );
    }

private:
    wait_type _w = INVALID_WAIT;
};

void wait_for( const wait &w );

wait                             wait_for_any( const std::vector<wait> &w );
template <typename... Args> wait wait_for_any( const wait &w, Args &&... ew )
{
    return wait_for_any( std::vector<wait>{ w, ew... } );
}

} // namespace base
