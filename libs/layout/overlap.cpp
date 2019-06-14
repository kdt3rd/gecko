// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "overlap.h"

#include <base/contract.h>

namespace layout
{
////////////////////////////////////////

overlap::overlap( void ) {}

////////////////////////////////////////

void overlap::compute_bounds( void )
{
    // Clean up areas that have been deleted.
    _areas.remove_if(
        []( const std::weak_ptr<area> &a ) { return a.expired(); } );

    coord minw = min_coord();
    coord minh = min_coord();
    coord maxw = min_coord();
    coord maxh = min_coord();

    for ( auto &wa: _areas )
    {
        auto a = wa.lock();
        if ( a )
        {
            a->compute_bounds();
            minw = std::max( minw, a->minimum_width() );
            minh = std::max( minh, a->minimum_height() );
            maxw = std::max( maxw, a->maximum_width() );
            maxh = std::max( maxh, a->maximum_height() );
        }
    }

    minw += _pad[0] + _pad[1];
    minh += _pad[2] + _pad[3];
    maxw += _pad[0] + _pad[1];
    maxh += _pad[2] + _pad[3];

    if ( _areas.empty() )
    {
        maxw = max_coord();
        maxh = max_coord();
    }

    set_minimum( minw, minh );
    set_maximum( maxw, maxh );
}

////////////////////////////////////////

void overlap::compute_layout( void )
{
    coord x = this->x() + _pad[0];
    coord y = this->y() + _pad[2];
    coord w = width() - _pad[0] - _pad[1];
    coord h = height() - _pad[2] - _pad[3];

    for ( auto &wa: _areas )
    {
        auto a = wa.lock();
        if ( a )
        {
            a->set_position( { x, y } );
            a->set_size( w, h );
            a->compute_layout();
        }
    }
}

////////////////////////////////////////

} // namespace layout
