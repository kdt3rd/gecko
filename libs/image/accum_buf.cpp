//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "accum_buf.h"
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

accum_buf::accum_buf( void )
{
}

////////////////////////////////////////

accum_buf::accum_buf( int w, int h )
	: _width( w ), _height( h )
{
	_mem = allocator::get().dbl_buffer( _stride, w, h );
}

////////////////////////////////////////

accum_buf::accum_buf( const accum_buf &o )
	: computed_base( o ),
	  _mem( o._mem ),
	  _width( o._width ),
	  _height( o._height ),
	  _stride( o._stride )
{
}

////////////////////////////////////////

accum_buf::accum_buf( accum_buf &&o )
	: computed_base( std::move( o ) ),
	  _mem( std::move( o._mem ) ),
	  _width( std::move( o._width ) ),
	  _height( std::move( o._height ) ),
	  _stride( std::move( o._stride ) )
{
}

////////////////////////////////////////

accum_buf &accum_buf::operator=( accum_buf &&o )
{
	adopt( std::move( o ) );
	_mem = std::move( o._mem );
	_width = std::move( o._width );
	_height = std::move( o._height );
	_stride = std::move( o._stride );
	return *this;
}

////////////////////////////////////////

accum_buf &accum_buf::operator=( const accum_buf &o )
{
	if ( this != &o )
	{
		internal_copy( o );
		_mem = o._mem;
		_width = o._width;
		_height = o._height;
		_stride = o._stride;
	}
	return *this;
}

////////////////////////////////////////

accum_buf::~accum_buf( void )
{
}

////////////////////////////////////////

void
accum_buf::check_compute( void ) const
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
		postcondition( _width == tmp.width() && _height == tmp.height(), "computed accumulation buffer does not match dimensions provided" );
		_mem = tmp._mem;
		_stride = tmp._stride;
		postcondition( _mem && _stride >= _width, "invalid computed bufer" );
		return;
	}

	throw_runtime( "Invalid access of uninitialized accumulation buffer" );
}

////////////////////////////////////////

engine::hash &operator<<( engine::hash &h, const accum_buf &p )
{
	if ( p.compute_hash( h ) )
		return h;

	h << p.width() << p.height();
	h.add( p.cdata(), p.buffer_size() );
	return h;
}

////////////////////////////////////////

} // namespace image

