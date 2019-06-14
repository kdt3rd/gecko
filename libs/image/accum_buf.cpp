// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "accum_buf.h"

#include <base/contract.h>

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

accum_buf::accum_buf( void ) {}

////////////////////////////////////////

accum_buf::accum_buf( int x1, int y1, int x2, int y2 )
    : _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 )
{
    _mem = allocator::get().dbl_buffer( _stride, width(), height() );
}

////////////////////////////////////////

accum_buf::accum_buf( const accum_buf &o )
    : computed_base( o )
    , _mem( o._mem )
    , _x1( o._x1 )
    , _y1( o._y1 )
    , _x2( o._x2 )
    , _y2( o._y2 )
    , _stride( o._stride )
{}

////////////////////////////////////////

accum_buf::accum_buf( accum_buf &&o )
    : computed_base( std::move( o ) )
    , _mem( std::move( o._mem ) )
    , _x1( o._x1 )
    , _y1( o._y1 )
    , _x2( o._x2 )
    , _y2( o._y2 )
    , _stride( std::move( o._stride ) )
{}

////////////////////////////////////////

accum_buf &accum_buf::operator=( accum_buf &&o )
{
    adopt( std::move( o ) );
    _mem    = std::move( o._mem );
    _x1     = std::move( o._x1 );
    _y1     = std::move( o._y1 );
    _x2     = std::move( o._x2 );
    _y2     = std::move( o._y2 );
    _stride = std::move( o._stride );
    return *this;
}

////////////////////////////////////////

accum_buf &accum_buf::operator=( const accum_buf &o )
{
    if ( this != &o )
    {
        internal_copy( o );
        _mem    = o._mem;
        _x1     = o._x1;
        _y1     = o._y1;
        _x2     = o._x2;
        _y2     = o._y2;
        _stride = o._stride;
    }
    return *this;
}

////////////////////////////////////////

accum_buf::~accum_buf( void ) {}

////////////////////////////////////////

void accum_buf::check_compute( void ) const
{
    if ( _mem )
        return;

    // pending() returns true if we need computation, but another
    // step may have already computed us, so it won't really be
    // pending any more. Instead, just check the graph and then
    // ask to compute to pull the value
    if ( _graph )
    {
        accum_buf tmp = base::any_cast<accum_buf>( compute() );
        postcondition(
            dims() == tmp.dims(),
            "computed accumulation buffer does not match dimensions provided" );
        postcondition(
            tmp._mem && tmp._stride >= width(), "invalid computed bufer" );
        _stride = tmp._stride;
        _mem    = tmp._mem;
        return;
    }

    throw_runtime( "Invalid access of uninitialized accumulation buffer" );
}

////////////////////////////////////////

engine::hash &operator<<( engine::hash &h, const accum_buf &p )
{
    if ( p.compute_hash( h ) )
        return h;

    h << typeid( p ).hash_code() << p.x1() << p.y1() << p.x2() << p.y2();
    h.add( p.cdata(), p.buffer_size() );
    return h;
}

////////////////////////////////////////

} // namespace image
