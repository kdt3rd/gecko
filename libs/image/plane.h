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

#pragma once

#include <algorithm>
#include "allocator.h"
#include <functional>
#include <base/contract.h>
#include <base/math_functions.h>
#include <engine/computed_value.h>
#include "op_registry.h"
#include "plane_buffer.h"
#include "plane_util.h"

////////////////////////////////////////

namespace image
{

/// @brief image plane abstraction to store a single channel of the image
///
/// always aligns the plane to at least 16 floats to
/// maximize alignment with AVX512
class plane : public engine::computed_base
{
public:
	typedef float value_type;
	plane( void );
	plane( int w, int h );
	plane( const engine::dimensions &d );
//	plane( media::image_buffer );
	template <typename... Args>
	inline plane( const base::cstring &opname, const engine::dimensions &d, Args &&... args )
		: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... ),
		  _width( static_cast<int>( d.x ) ),
		  _width_m1( static_cast<int>( d.x ) - 1 ),
		  _height( static_cast<int>( d.y ) ),
		  _height_m1( static_cast<int>( d.y ) - 1 )
	{
	}

	plane( const plane & );
	plane( plane && );
	plane &operator=( const plane & );
	plane &operator=( plane && );
	~plane( void );

	inline bool valid( void ) const { return _width > 0 && _height > 0; }

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r;
		r.x = static_cast<engine::dimensions::value_type>( width() );
		r.y = static_cast<engine::dimensions::value_type>( height() );
		return r;
	}

	inline int width( void ) const { return _width; }
	inline int height( void ) const { return _height; }

	inline int lastx( void ) const { return _width_m1; }
	inline int lasty( void ) const { return _height_m1; }

	inline int stride( void ) const { check_compute(); return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline operator const_plane_buffer( void ) const { check_compute(); return const_plane_buffer( cdata(), width(), height(), _width_m1, _height_m1, stride() ); }

	inline operator plane_buffer( void ) { check_compute(); return plane_buffer( data(), width(), height(), _width_m1, _height_m1, stride() ); }

	inline value_type *data( void ) { check_compute(); return _mem.get(); }
	inline const value_type *data( void ) const { check_compute(); return _mem.get(); }
	inline const value_type *cdata( void ) const { check_compute(); return _mem.get(); }
	inline value_type *line( int y ) { return data() + y * stride(); }
	inline const value_type *line( int y ) const { return cdata() + y * stride(); }

	inline bool in_bounds_x( int x ) const { return x >= 0 && x < _width; }
	inline bool in_bounds_y( int y ) const { return y >= 0 && y < _height; }
	inline bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a reference to a value for setting purposes
	inline value_type &get( int x, int y ) { return *( line( y ) + x ); }
	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline value_type get( int x, int y ) const { return *( line( y ) + x ); }
	/// Alternate explicit set method
	inline void set( int x, int y, value_type v ) { get( x, y ) = v; }

	/// duplicates the plane, including copying the pixel values
	/// NB: if the plane has not yet been computed, will compute prior to copy
	plane copy( void ) const;
	/// initializes a new plane with the same dimensions, but does NOT
	/// copy the memory or any compute parameters
	plane clone( void ) const;

private:
	inline void check_compute( void ) const
	{
		if ( _mem )
			return;

		// pending() returns true if we need computation, but another
		// step may have already computed us, so it won't really be
		// pending any more. Instead, just check the graph and then
		// ask to compute to pull the value
		if ( _graph )
		{
			plane tmp = base::any_cast<plane>( compute() );
			postcondition( _width == tmp.width() && _height == tmp.height(), "computed plane does not match dimensions provided" );
			_mem = tmp._mem;
			_stride = tmp._stride;
			postcondition( _mem && _stride >= _width, "invalid computed plane" );
			return;
		}

		throw_runtime( "Invalid access of uninitialized plane" );
	}

	mutable std::shared_ptr<value_type> _mem;
	int _width = 0;
	int _width_m1 = 0;
	int _height = 0;
	int _height_m1 = 0;
	mutable int _stride = 0;
};

engine::hash &operator<<( engine::hash &h, const plane &p );

} // namespace image



