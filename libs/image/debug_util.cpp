// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "debug_util.h"

#include <base/contract.h>
#include <cmath>

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

bool check_nan( const plane &p )
{
    int w = p.width();
    int h = p.height();
    for ( int y = 0; y != h; ++y )
    {
        const float *line = p.line( y );
        for ( int x = 0; x != w; ++x )
        {
            if ( std::isnan( line[x] ) )
                return true;
        }
    }
    return false;
}

void check_nan_and_report( const plane &p )
{
    int w = p.width();
    int h = p.height();
    for ( int y = 0; y != h; ++y )
    {
        const float *line = p.line( y );
        for ( int x = 0; x != w; ++x )
        {
            if ( std::isnan( line[x] ) )
            {
                std::cout << "first NaN detected at (" << x << ", " << y << ")"
                          << std::endl;
                return;
            }
        }
    }
}

////////////////////////////////////////

} // namespace image
