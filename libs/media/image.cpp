//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "image.h"

////////////////////////////////////////

namespace media
{

////////////////////////////////////////

image::image( void )
{
}

////////////////////////////////////////

image::image( const area_rect &area )
    : _full_area( area ), _active_area( area )
{
}

////////////////////////////////////////

image::~image( void )
{
}

////////////////////////////////////////

void image::color_state( const color::state &s )
{
    _color_state = s;
}

////////////////////////////////////////

image::plane_list image::available_planes( void ) const
{
    image::plane_list r;
    r.reserve( _planes.size() );
    for ( auto &p: _planes )
        r.push_back( p._name );
    return r;
}

////////////////////////////////////////

std::pair<int64_t, int64_t> image::compute_preferred_chunk( void ) const
{
    return std::make_pair(
        static_cast<int64_t>( _active_area.width() ),
        static_cast<int64_t>( _active_area.height() ) );
}

} // media



