// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "vector_field.h"
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

vector_field::vector_field( void )
{
}

////////////////////////////////////////

vector_field::vector_field( int x1, int y1, int x2, int y2, bool isAbsolute )
	: _u( x1, y1, x2, y2 ), _v( x1, y1, x2, y2 ), _absolute( isAbsolute )
{
}

////////////////////////////////////////

vector_field::vector_field( const engine::dimensions &d, bool isAbsolute )
	: vector_field( static_cast<int>( d.x1 ), static_cast<int>( d.y1 ),
					static_cast<int>( d.x2 ), static_cast<int>( d.y2 ),
					isAbsolute )
{
}

////////////////////////////////////////

vector_field::vector_field( const plane &u, const plane &v, bool isAbsolute )
	: _u( u ), _v( v ), _absolute( isAbsolute )
{
	precondition( u.dims() == v.dims(), "vector_field must have u and v of same size, received u {0}, v {1}", u.dims(), u.dims() );
}

////////////////////////////////////////

vector_field::vector_field( plane &&u, plane &&v, bool isAbsolute )
	: _u( std::move( u ) ), _v( std::move( v ) ), _absolute( isAbsolute )
{
	precondition( _u.dims() == _v.dims(), "vector_field must have u and v of same size, received u {0}, v {1}", u.dims(), u.dims() );
}

////////////////////////////////////////

vector_field::~vector_field( void )
{
}

////////////////////////////////////////

vector_field::vector_field( const vector_field &o )
	: computed_base( o ), _u( o._u ), _v( o._v ), _absolute( o._absolute )
{
}

////////////////////////////////////////

vector_field::vector_field( vector_field &&o )
	: computed_base( std::move( o ) ),
	  _u( std::move( o._u ) ), _v( std::move( o._v ) ),
	  _absolute( o._absolute )
{
}

////////////////////////////////////////

vector_field &vector_field::operator=( const vector_field &o )
{
	if ( this != &o )
	{
		internal_copy( o );
		_u = o._u;
		_v = o._v;
		_absolute = o._absolute;
	}
	return *this;
}

////////////////////////////////////////

vector_field &vector_field::operator=( vector_field &&o )
{
	adopt( std::move( o ) );
	_u = std::move( o._u );
	_v = std::move( o._v );
	_absolute = o._absolute;
	return *this;
}

////////////////////////////////////////

vector_field
vector_field::create( const plane &a, const plane &b, bool isAbsolute )
{
	return vector_field( a, b, isAbsolute );
}

////////////////////////////////////////

vector_field
vector_field::create( plane &&a, plane &&b, bool isAbsolute )
{
	return vector_field( std::move( a ), std::move( b ), isAbsolute );
}

////////////////////////////////////////

engine::hash &operator<<( engine::hash &h, const vector_field &v )
{
	if ( v.compute_hash( h ) )
		return h;

	h << typeid(v).hash_code() << v.is_absolute() << v.u() << v.v();
	return h;
}


////////////////////////////////////////


} // namespace image

