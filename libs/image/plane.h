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

	plane( const plane & ) = default;
	plane( plane && );
	plane &operator=( const plane & ) = default;
	plane &operator=( plane && );
	~plane( void );

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r;
		r.x = static_cast<engine::dimensions::value_type>( width() );
		r.y = static_cast<engine::dimensions::value_type>( height() );
		return r;
	}

	inline int width( void ) const { return _width; }
	inline int height( void ) const { return _height; }

	inline int stride( void ) const { check_compute(); return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(float); }

	inline float *data( void ) { check_compute(); return _mem.get(); }
	inline const float *cdata( void ) const { check_compute(); return _mem.get(); }
	inline float *line( int y ) { return data() + y * stride(); }
	inline const float *line( int y ) const { return cdata() + y * stride(); }

	inline bool in_bounds_x( int x ) const { return x >= 0 && x < _width; }
	inline bool in_bounds_y( int y ) const { return y >= 0 && y < _height; }
	inline bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	inline float &get( int x, int y ) { return *( line( y ) + x ); }
	inline float get( int x, int y ) const { return *( line( y ) + x ); }
	inline void set( int x, int y, float v ) { get( x, y ) = v; }

	inline float get_zero( int x, int y ) const
	{
		if ( x < 0 || x >= _width )
			return 0.F;
		if ( y < 0 || y >= _height )
			return 0.F;
		return get( x, y );
	}
	inline float get_hold( int x, int y ) const
	{
		return get( std::max( 0, std::min( _width_m1, x ) ),
					std::max( 0, std::min( _height_m1, y ) ) );
	}
	inline float get_mirror( int x, int y ) const
	{
		while ( ! in_bounds_x( x ) )
		{
			if ( x < 0 )
				x = -x;
			if ( x > _width_m1 )
				x = _width_m1 * 2 - x;
		}
		while ( ! in_bounds_y( y ) )
		{
			if ( y < 0 )
				y = -y;
			if ( y > _height_m1 )
				y = _height_m1 * 2 - y;
		}

		return get( x, y );
	}

	/// bilinear sample, treating requests outside as zero
	inline float bilinear_zero( float x, float y ) const
	{
		int ix = static_cast<int>( x );
		if ( x < 0.F )
			--ix;
		float percBx = x - static_cast<float>( ix );
		int iy = static_cast<int>( y );
		if ( y < 0.F )
			--iy;
		float percBy = y - static_cast<float>( iy );

		float a00 = get_zero( ix, iy );
		float a10 = get_zero( ix + 1, iy );
		float a01 = get_zero( ix, iy + 1 );
		float a11 = get_zero( ix + 1, iy + 1 );
		float t0 = base::lerp( a00, a10, percBx );
		float t1 = base::lerp( a01, a11, percBx );
		return base::lerp( t0, t1, percBy );
	}

	/// bilinear sample, holding edge
	inline float bilinear_hold( float x, float y ) const
	{
		int ix = static_cast<int>( x );
		if ( x < 0.F )
			--ix;
		float percBx = x - static_cast<float>( ix );
		int iy = static_cast<int>( y );
		if ( y < 0.F )
			--iy;
		float percBy = y - static_cast<float>( iy );

		float a00 = get_hold( ix, iy );
		float a10 = get_hold( ix + 1, iy );
		float a01 = get_hold( ix, iy + 1 );
		float a11 = get_hold( ix + 1, iy + 1 );
		float t0 = base::lerp( a00, a10, percBx );
		float t1 = base::lerp( a01, a11, percBx );
		return base::lerp( t0, t1, percBy );
	}

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

		if ( pending() )
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

	mutable std::shared_ptr<float> _mem;
	int _width = 0;
	int _width_m1 = 0;
	int _height = 0;
	int _height_m1 = 0;
	mutable int _stride = 0;
};

engine::hash &operator<<( engine::hash &h, const plane &p );

} // namespace image



