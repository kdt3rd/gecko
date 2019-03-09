// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "image.h"
#include <base/allocator.h>
#include "image_buffer.h"

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

void image::create_buffers(
    std::vector<image_buffer> &planes,
    base::allocator &a,
    bool preferred_chunk ) const
{
    // TODO: re-use existing???
    planes.clear();
    if ( empty() )
        return;

    planes.resize( size() );
    area_rect ar = active_area();
    if ( preferred_chunk )
    {
        auto pcs = preferred_chunk_size();
        ar.set_size( pcs.first, pcs.second );
    }

    if ( interleaved() )
    {
        std::shared_ptr<void> ibuf;

        size_t chans = size();
        const plane_layout &pl = _planes.front()._layout;

        int16_t bits = static_cast<int16_t>( pl._bits );
        size_t bpc = bits / 8;
		size_t linebytes = static_cast<size_t>( ar.width() * chans ) * bpc;
		int64_t xstride = static_cast<int64_t>( bpc ) * chans;
		int64_t ystride = static_cast<int64_t>( linebytes );
        base::endianness endi = pl._endian;
		const bool isf = pl._floating;
		const bool isu = pl._unsigned;

        // TODO: alignment?
        ibuf = a.allocate( ystride * ar.height() );

        for ( size_t p = 0; p != planes.size(); ++p )
        {
            precondition( pl._bits == _planes[p]._layout._bits, "unhandled mismatch of bits for interleaved buffer" );
            // TODO: fix this for 4:2:2 buffers!
            precondition( pl._ysubsample == _planes[p]._layout._ysubsample, "unhandled mismatch of y subsample for interleaved buffer" );
            precondition( pl._xsubsample == _planes[p]._layout._xsubsample, "unhandled mismatch of x subsample for interleaved buffer" );
            planes[p] = image_buffer(
                ibuf, bits, ar, xstride * 8, ystride * 8, bits * p,
                endi, isf, isu );
        }
    }
    else
    {
        for ( size_t p = 0; p != planes.size(); ++p )
        {
            const plane_layout &pl = layout( p );

            int16_t bits = static_cast<int16_t>( pl._bits );
            size_t bpc = bits / 8;
            size_t linebytes = static_cast<size_t>( ar.width() ) * bpc;
            int64_t xstride = static_cast<int64_t>( bpc );
            int64_t ystride = static_cast<int64_t>( linebytes );
            base::endianness endi = pl._endian;
            const bool isf = pl._floating;
            const bool isu = pl._unsigned;

            // TODO: alignment?
            planes[p] = image_buffer(
                a.allocate( ystride * ar.height() ),
                bits, ar, xstride * 8, ystride * 8, 0,
                endi, isf, isu );
        }
    }
}

////////////////////////////////////////

void image::extract_plane( size_t plane, image_buffer &pbuf )
{
    // TODO: need to handle cache of all planes
    if ( interleaved() )
        throw_not_yet();

    fill_plane( plane, pbuf );
}

////////////////////////////////////////

void image::extract_image( std::vector<image_buffer> &planes )
{
    precondition( size() == planes.size(), "expect image buffer list same size as plane count" );
    // TODO: keep this or make it optional?
    for ( size_t p = 0; p != planes.size(); ++p )
    {
        image_buffer &ib = planes[p];
        const plane_layout &pl = _planes[p]._layout;
        if ( ! ib.raw() )
            throw_runtime( "Invalid un-initialized image buffer for plane {0} ('{1}')",
                           p, plane_name( p ) );

        if ( pl._bits != ib.bits() )
            throw_runtime( "Mis-match of bits for plane {0} ('{1}'): {2} vs {3}",
                           p, plane_name( p ), pl._bits, ib.bits() );
    }
    fill_image( planes );
}

////////////////////////////////////////

std::pair<int64_t, int64_t> image::compute_preferred_chunk( void ) const
{
    return std::make_pair(
        static_cast<int64_t>( _active_area.width() ),
        static_cast<int64_t>( _active_area.height() ) );
}

} // media



