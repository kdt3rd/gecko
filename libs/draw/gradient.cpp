// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "gradient.h"

#include <algorithm>
#include <cmath>

namespace draw
{
////////////////////////////////////////

void gradient::sort( void )
{
    auto cmp = []( const stop_type &a, const stop_type &b ) {
        return a.first < b.first;
    };
    std::sort( _stops.begin(), _stops.end(), cmp );
}

////////////////////////////////////////

color gradient::sample( value_type v ) const
{
    size_t i = 0;
    while ( i < _stops.size() )
    {
        if ( _stops[i].first > v )
            break;
        ++i;
    }

    if ( i < _stops.size() )
    {
        if ( i == 0 )
            return _stops[0].second;

        value_type range = _stops[i].first - _stops[i - 1].first;
        value_type mix   = ( _stops[i].first - v ) / range;

        return color::mix( _stops[i].second, _stops[i - 1].second, mix );
    }

    return _stops.back().second;
}

////////////////////////////////////////

} // namespace draw
