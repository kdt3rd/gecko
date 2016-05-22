//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
	: _mem( allocator::get().buffer( _stride, w, h ) ),
	  _width( w ), _width_m1( w - 1 ), _height( h ), _height_m1( h - 1 )
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

	h << p.width() << p.height();
	h.add( p.cdata(), p.buffer_size() );
	return h;
}

////////////////////////////////////////

} // image



