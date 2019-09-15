// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "data.h"

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

data::data( void ) {}

////////////////////////////////////////

data::data( const area_rect &area ) : _full_area( area ), _active_area( area )
{}

////////////////////////////////////////

data::~data( void ) {}

////////////////////////////////////////

void data::color_state( const color::state &s ) { _color_state = s; }

////////////////////////////////////////

data::plane_list data::available_planes( void ) const
{
    plane_list r;
    r.reserve( _planes.size() );
    for ( auto &p: _planes )
        r.push_back( p );
    return r;
}

////////////////////////////////////////

std::pair<int64_t, int64_t> data::compute_preferred_chunk( void ) const
{
    return std::make_pair(
        static_cast<int64_t>( _active_area.width() ),
        static_cast<int64_t>( _active_area.height() ) );
}

////////////////////////////////////////

} // namespace media
