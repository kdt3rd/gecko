//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
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
	plane( int x1, int y1, int x2, int y2 );
	plane( const engine::dimensions &d );
//	plane( media::image_buffer );
	template <typename... Args>
	inline plane( const base::cstring &opname, const engine::dimensions &d, Args &&... args )
		: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... ),
		  _x1( d.x1 ), _y1( d.y1 ), _x2( d.x2 ), _y2( d.y2 )
	{
	}

	plane( const plane & );
	plane( plane && );
	plane &operator=( const plane & );
	plane &operator=( plane && );
	~plane( void );

	inline bool valid( void ) const { return _x2 > _x1 && _y2 > _y1; }

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r;
		r.x1 = static_cast<engine::dimensions::value_type>( _x1 );
		r.y1 = static_cast<engine::dimensions::value_type>( _y1 );
		r.x2 = static_cast<engine::dimensions::value_type>( _x2 );
		r.y2 = static_cast<engine::dimensions::value_type>( _y2 );
		r.planes = 1;
		r.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(float) );
		return r;
	}

	inline int width( void ) const { return _x2 - _x1 + 1; }
	inline int height( void ) const { return _y2 - _y1 + 1; }

	inline int x1( void ) const { return _x1; }
	inline int y1( void ) const { return _y1; }
	inline int x2( void ) const { return _x2; }
	inline int y2( void ) const { return _y2; }

	inline int stride( void ) const { check_compute(); return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline operator const_plane_buffer( void ) const
	{
		if ( _graph )
			check_compute();

		if ( valid() )
			return const_plane_buffer( cdata(), _x1, _y1, _x2, _y2, stride() );

		return const_plane_buffer( nullptr, 0, 0, 0, 0, 0 );
	}

	inline operator plane_buffer( void )
	{
		if ( _graph )
			check_compute();

		if ( valid() )
			return plane_buffer( data(), _x1, _y1, _x2, _y2, stride() );
		return plane_buffer( nullptr, 0, 0, 0, 0, 0 );
	}

	inline value_type *data( void ) { check_compute(); return _mem.get(); }
	inline const value_type *data( void ) const { check_compute(); return _mem.get(); }
	inline const value_type *cdata( void ) const { check_compute(); return _mem.get(); }
	inline value_type *line( int y ) { return data() + ( y - _y1 ) * stride(); }
	inline const value_type *line( int y ) const { return cdata() + ( y - _y1 ) * stride(); }

	inline bool in_bounds_x( int x ) const { return x >= _x1 && x <= _x2; }
	inline bool in_bounds_y( int y ) const { return y >= _y1 && y <= _y2; }
	inline bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a reference to a value for setting purposes
	inline value_type &get( int x, int y ) { return *( line( y ) + ( x - _x1 ) ); }
	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline value_type get( int x, int y ) const { return *( line( y ) + ( x - _x1 ) ); }
	/// Alternate explicit set method
	inline void set( int x, int y, value_type v ) { get( x, y ) = v; }

	/// duplicates the plane, including copying the pixel values
	/// NB: if the plane has not yet been computed, will compute prior to copy
	plane copy( void ) const;
	/// initializes a new plane with the same dimensions, but does NOT
	/// copy the memory or any compute parameters
	plane clone( void ) const;

private:
	void run_compute( void ) const;
	inline void check_compute( void ) const
	{
		if ( _mem )
			return;

		run_compute();
	}

	mutable std::shared_ptr<value_type> _mem;
	int _x1 = 0;
	int _y1 = 0;
	int _x2 = 0;
	int _y2 = 0;
	mutable int _stride = 0;
};

engine::hash &operator<<( engine::hash &h, const plane &p );

inline void
swap( plane &a, plane &b )
{
	plane tmp = std::move( a );
	a = std::move( b );
	b = std::move( tmp );
}

////////////////////////////////////////

} // namespace image



