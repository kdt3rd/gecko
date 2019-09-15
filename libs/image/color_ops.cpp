// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "color_ops.h"

#include "threading.h"

#include <color/color.h>
#include <sstream>

////////////////////////////////////////

namespace color
{
/// \todo { do something smarter (fewer memory allocs) here than serialize to a string }
engine::hash &operator<<( engine::hash &h, const state &p )
{
    std::stringstream tmp;
    tmp << p;
    h << tmp.str();
    return h;
}

} // namespace color

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

static void colorspace_line(
    size_t,
    int                 s,
    int                 e,
    image_buf &         ret,
    const image_buf &   src,
    const color::state &from,
    const color::state &to )
{
    plane &      xOut = ret[0];
    plane &      yOut = ret[1];
    plane &      zOut = ret[2];
    const plane &xIn  = src[0];
    const plane &yIn  = src[1];
    const plane &zIn  = src[2];

    int w = ret.width();
    for ( int y = s; y < e; ++y )
    {
        float *      xLine   = xOut.line( y );
        float *      yLine   = yOut.line( y );
        float *      zLine   = zOut.line( y );
        const float *xInLine = xIn.line( y );
        const float *yInLine = yIn.line( y );
        const float *zInLine = zIn.line( y );

        for ( int x = 0; x < w; ++x )
        {
            float xV = xInLine[x];
            float yV = yInLine[x];
            float zV = zInLine[x];

            /// \todo { extract the logic inside here out to a higher level... }
            color::convert( xV, yV, zV, from, to, 32 );

            xLine[x] = xV;
            yLine[x] = yV;
            zLine[x] = zV;
        }
        for ( size_t i = 3; i < ret.size(); ++i )
            std::copy(
                src[i].line( y ), src[i].line( y ) + w, ret[i].line( y ) );
    }
}

////////////////////////////////////////

static image_buf
compute_colorspace( const image_buf &a, color::state from, color::state to )
{
    image_buf ret;
    for ( size_t i = 0; i != a.size(); ++i )
        ret.add_plane( plane( a.x1(), a.y1(), a.x2(), a.y2() ) );

    threading::get().dispatch(
        std::bind(
            colorspace_line,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( ret ),
            std::cref( a ),
            std::cref( from ),
            std::cref( to ) ),
        a.y1(),
        a.height() );
    return ret;
}

////////////////////////////////////////

image_buf colorspace(
    const image_buf &a, const color::state &from, const color::state &to )
{
    if ( a.size() < 3 )
        throw std::logic_error(
            "Attempt to convert color space on an image with fewer than 3 planes" );

    engine::dimensions d = a.dims();
    d.planes             = a.size();
    d.images             = 1;
    return image_buf( "i.colorspace", d, a, from, to );
}

////////////////////////////////////////

void add_color_ops( engine::registry &r )
{
    using namespace engine;

    r.register_constant<color::state>();
    // we will just do a threaded op so we can optimize the from / to operations applied once
    r.add( op( "i.colorspace", compute_colorspace, op::threaded ) );
}

} // namespace image
