//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "event.h"
#include "window.h"

namespace gui
{

coord event::from_native_horiz( const platform::coord_type &c ) const
{
    return _w.from_native_horiz( c );
}

coord event::from_native_vert( const platform::coord_type &c ) const
{
    return _w.from_native_vert( c );
}

point event::from_native( const platform::point &p ) const
{
    return _w.from_native( p );
}

point event::from_native( platform::coord_type x, platform::coord_type y ) const
{
    return _w.from_native( platform::point( x, y ) );
}

size event::from_native( const platform::size &s ) const
{
    return _w.from_native( s );
}

rect event::from_native( const platform::rect &r ) const
{
    return _w.from_native( r );
}

} // namespace gui
