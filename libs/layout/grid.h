// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "layout.h"

#include <vector>

namespace layout
{
////////////////////////////////////////

class grid : public layout
{
public:
    using area_ref = const std::shared_ptr<area> &;
    size_t add_columns( size_t n, coord flex = coord( 1 ), int32_t pri = 0 );
    size_t add_rows( size_t n, coord flex = coord( 1 ), int32_t pri = 0 );

    void add( area_ref a, size_t x, size_t y, size_t w, size_t h );
    void add( area_ref a, int x, int y, int w, int h )
    {
        add( a,
             static_cast<size_t>( x ),
             static_cast<size_t>( y ),
             static_cast<size_t>( w ),
             static_cast<size_t>( h ) );
    }

    void add( area_ref a, size_t x, size_t y )
    {
        add( a, x, y, size_t( 1 ), size_t( 1 ) );
    }
    void add( area_ref a, int x, size_t y ) { add( a, static_cast<size_t>( x ), y ); }
    void add( area_ref a, size_t x, int y ) { add( a, x, static_cast<size_t>( y ) ); }

    void compute_bounds( void ) override;

    void compute_layout( void ) override;

private:
    struct cell
    {
        cell( const std::shared_ptr<area> &a, size_t x, size_t y, size_t w, size_t h )
            : _area( a ), _x( x ), _y( y ), _w( w ), _h( h )
        {}

        std::weak_ptr<area> _area;
        size_t              _x, _y, _w, _h;
    };

    std::list<cell>                    _areas;
    std::vector<std::shared_ptr<area>> _cols;
    std::vector<std::shared_ptr<area>> _rows;
};

////////////////////////////////////////

} // namespace layout
