//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "plane.h"

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane::plane( void )
{
}

////////////////////////////////////////

plane::plane( int w, int h )
	: _width( w ), _width_m1( w - 1 ), _height( h ), _height_m1( h - 1 )
{
	_mem = allocator::get().buffer( _stride, w, h );
}

////////////////////////////////////////

plane::plane( const engine::dimensions &d )
	: plane( static_cast<int>( d.x ), static_cast<int>( d.y ) )
{
}

////////////////////////////////////////

plane::plane( const plane &o )
	: computed_base( o ),
	  _mem( o._mem ),
	  _width( o._width ),
	  _width_m1( o._width_m1 ),
	  _height( o._height ),
	  _height_m1( o._height_m1 ),
	  _stride( o._stride )
{
}

////////////////////////////////////////

plane::plane( plane &&o )
	: computed_base( std::move( o ) ),
	  _mem( std::move( o._mem ) ),
	  _width( std::move( o._width ) ),
	  _width_m1( std::move( o._width_m1 ) ),
	  _height( std::move( o._height ) ),
	  _height_m1( std::move( o._height_m1 ) ),
	  _stride( std::move( o._stride ) )
{
}

////////////////////////////////////////

plane::~plane( void )
{
}

////////////////////////////////////////

plane &plane::operator=( plane &&o )
{
	adopt( std::move( o ) );
	_mem = std::move( o._mem );
	_width = std::move( o._width );
	_width_m1 = std::move( o._width_m1 );
	_height = std::move( o._height );
	_height_m1 = std::move( o._height_m1 );
	_stride = std::move( o._stride );
	return *this;
}

////////////////////////////////////////

plane &plane::operator=( const plane &o )
{
	if ( this != &o )
	{
		internal_copy( o );
		_mem = o._mem;
		_width = o._width;
		_width_m1 = o._width_m1;
		_height = o._height;
		_height_m1 = o._height_m1;
		_stride = o._stride;
	}
	return *this;
}

////////////////////////////////////////

plane
plane::copy( void ) const
{
	plane r( width(), height() );

	memcpy( r.data(), cdata(), buffer_size() );

	return r;
}

////////////////////////////////////////

plane
plane::clone( void ) const
{
	return plane( width(), height() );
}

////////////////////////////////////////

engine::hash &operator<<( engine::hash &h, const plane &p )
{
	if ( p.compute_hash( h ) )
		return h;

	h << typeid(p).hash_code() << p.width() << p.height();
	h.add( p.cdata(), p.buffer_size() );
	return h;
}

////////////////////////////////////////

} // image



