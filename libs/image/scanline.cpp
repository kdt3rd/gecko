// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "scanline.h"

#include <base/pointer.h>

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

scanline::scanline( int offx, const float *b, int w, int s, bool dup )
    : _ref_ptr( b ), _offset( offx ), _width( w ), _stride( s )
{
    if ( dup )
    {
        _ptr = allocator::get().scanline( _stride, _width );
        if ( _ref_ptr )
            std::copy( _ref_ptr, _ref_ptr + _width, _ptr.get() );
        _ref_ptr = _ptr.get();
    }
}

////////////////////////////////////////

scanline::scanline( int offx, float *b, int w, int s )
    : _ptr( b, base::no_deleter() )
    , _ref_ptr( b )
    , _offset( offx )
    , _width( w )
    , _stride( s )
{}

////////////////////////////////////////

scanline::scanline( int offx, int w )
    : _ptr( allocator::get().scanline( _stride, w ) )
    , _ref_ptr( _ptr.get() )
    , _offset( offx )
    , _width( w )
{}

////////////////////////////////////////

void scanline::swap( scanline &o )
{
    std::swap( _ptr, o._ptr );
    std::swap( _ref_ptr, o._ref_ptr );
    std::swap( _offset, o._offset );
    std::swap( _width, o._width );
    std::swap( _stride, o._stride );
}

////////////////////////////////////////

} // namespace image
