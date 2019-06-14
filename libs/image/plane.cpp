// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "plane.h"

#include "plane_buffer.h"
#include "threading.h"

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

plane::plane( void ) {}

////////////////////////////////////////

plane::plane( int x1, int y1, int x2, int y2 )
    : _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 )
{
    _mem = allocator::get().buffer( _stride, width(), height() );
}

////////////////////////////////////////

plane::plane( const engine::dimensions &d )
    : plane(
          static_cast<int>( d.x1 ),
          static_cast<int>( d.y1 ),
          static_cast<int>( d.x2 ),
          static_cast<int>( d.y2 ) )
{}

////////////////////////////////////////

plane::plane( const plane &o )
    : computed_base( o )
    , _mem( o._mem )
    , _x1( o._x1 )
    , _y1( o._y1 )
    , _x2( o._x2 )
    , _y2( o._y2 )
    , _stride( o._stride )
{
    if ( _mem )
        clear_graph();
}

////////////////////////////////////////

plane::plane( plane &&o )
    : computed_base( std::move( o ) )
    , _mem( std::move( o._mem ) )
    , _x1( std::move( o._x1 ) )
    , _y1( std::move( o._y1 ) )
    , _x2( std::move( o._x2 ) )
    , _y2( std::move( o._y2 ) )
    , _stride( std::move( o._stride ) )
{
    if ( _mem )
        clear_graph();
}

////////////////////////////////////////

plane::~plane( void ) {}

////////////////////////////////////////

plane &plane::operator=( plane &&o )
{
    _x1     = std::move( o._x1 );
    _y1     = std::move( o._y1 );
    _x2     = std::move( o._x2 );
    _y2     = std::move( o._y2 );
    _mem    = std::move( o._mem );
    _stride = std::move( o._stride );

    adopt( std::move( o ) );
    if ( _mem )
        clear_graph();

    return *this;
}

////////////////////////////////////////

plane &plane::operator=( const plane &o )
{
    if ( this != &o )
    {
        _mem    = o._mem;
        _stride = o._stride;
        _x1     = o._x1;
        _y1     = o._y1;
        _x2     = o._x2;
        _y2     = o._y2;

        internal_copy( o );
        if ( _mem )
            clear_graph();
    }
    return *this;
}

////////////////////////////////////////

static void copyPlane(
    size_t, int s, int e, plane_buffer &out, const const_plane_buffer &src )
{
    if ( out.stride() == src.stride() )
    {
        memcpy(
            out.line( s ),
            src.line( s ),
            static_cast<size_t>( ( e - s ) * out.stride() ) * sizeof( float ) );
    }
    else
    {
        for ( int y = s; y < e; ++y )
            memcpy(
                out.line( y ),
                src.line( y ),
                static_cast<size_t>( out.width() ) * sizeof( float ) );
    }
}

////////////////////////////////////////

plane plane::copy( void ) const
{
    plane r( x1(), y1(), x2(), y2() );

    plane_buffer       out = r;
    const_plane_buffer in  = *this;
    threading::get().dispatch(
        std::bind(
            copyPlane,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( out ),
            std::cref( in ) ),
        r );

    return r;
}

////////////////////////////////////////

plane plane::clone( void ) const { return plane( x1(), y1(), x2(), y2() ); }

////////////////////////////////////////

void plane::run_compute( void ) const
{
    // pending() returns true if we need computation, but another
    // step may have already computed us, so it won't really be
    // pending any more. Instead, just check the graph and then
    // ask to compute to pull the value
    if ( _graph )
    {
        plane tmp = base::any_cast<plane>( compute() );
        postcondition(
            dims() == tmp.dims(),
            "computed plane does not match dimensions provided" );
        postcondition(
            tmp._mem && tmp._stride >= width(), "invalid computed plane" );
        _stride = tmp._stride;
        _mem    = tmp._mem;
        return;
    }

    throw_runtime( "Invalid access of uninitialized plane" );
}

////////////////////////////////////////

engine::hash &operator<<( engine::hash &h, const plane &p )
{
    if ( p.compute_hash( h ) )
        return h;

    h << typeid( p ).hash_code() << p.x1() << p.y1() << p.x2() << p.y2();
    if ( p.valid() )
        h << reinterpret_cast<intptr_t>( p.cdata() );
    //		h.add( p.cdata(), p.buffer_size() );
    return h;
}

////////////////////////////////////////

} // namespace image
