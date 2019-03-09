// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <base/contract.h>

////////////////////////////////////////

namespace image
{

class plane_buffer
{
public:
	typedef float value_type;

	inline plane_buffer( void ) {}
	inline plane_buffer( float *p, int x1, int y1, int x2, int y2, int s )
		: _ptr( p ), _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 ), _stride( s )
	{}

	inline bool valid( void ) const { return _x2 > _x1 && _y2 > _y1; }

	inline int width( void ) const { return _x2 - _x1 + 1; }
	inline int height( void ) const { return _y2 - _y1 + 1; }

	inline int x1( void ) const { return _x1; }
	inline int y1( void ) const { return _y1; }
	inline int x2( void ) const { return _x2; }
	inline int y2( void ) const { return _y2; }

	inline int stride( void ) const { return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline value_type *data( void ) { return _ptr; }
	inline const value_type *data( void ) const {  return _ptr; }
	inline const value_type *cdata( void ) const { return _ptr; }

	inline value_type *line( int y ) { return data() + ( y - _y1 ) * stride(); }
	inline const value_type *line( int y ) const { return cdata() + ( y - _y1 ) * stride(); }

	inline bool in_bounds_x( int x ) const { return x >= _x1 && x <= _x2; }
	inline bool in_bounds_y( int y ) const { return y >= _y1 && y <= _y2; }
	inline bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a reference to a value for setting purposes
	/// @sa plane_util.h for other access methods
	inline value_type &get( int x, int y ) { return *( line( y ) + ( x - _x1 ) ); }
	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline value_type get( int x, int y ) const { return *( line( y ) + ( x - _x1 ) ); }
	/// Alternate explicit set method
	inline void set( int x, int y, value_type v ) { get( x, y ) = v; }

private:
	float *_ptr = nullptr;
	int _x1 = 0;
	int _y1 = 0;
	int _x2 = 0;
	int _y2 = 0;
	int _stride = 0;
};


class const_plane_buffer
{
public:
	typedef float value_type;

	inline constexpr const_plane_buffer( void ) {}
	inline constexpr const_plane_buffer( const float *p, int x1, int y1, int x2, int y2, int s )
		: _ptr( p ), _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 ), _stride( s )
	{}

	inline constexpr bool valid( void ) const { return _x2 > _x1 && _y2 > _y1; }

	inline constexpr int width( void ) const { return _x2 - _x1 + 1; }
	inline constexpr int height( void ) const { return _y2 - _y1 + 1; }

	inline constexpr int x1( void ) const { return _x1; }
	inline constexpr int y1( void ) const { return _y1; }
	inline constexpr int x2( void ) const { return _x2; }
	inline constexpr int y2( void ) const { return _y2; }

	inline constexpr int stride( void ) const { return _stride; }

	inline constexpr size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline constexpr const value_type *data( void ) const {  return _ptr; }
	inline constexpr const value_type *cdata( void ) const { return _ptr; }

	inline constexpr const value_type *line( int y ) const { return cdata() + ( y - _y1 ) * stride(); }

	inline constexpr bool in_bounds_x( int x ) const { return x >= _x1 && x <= _x2; }
	inline constexpr bool in_bounds_y( int y ) const { return y >= _y1 && y <= _y2; }
	inline constexpr bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline constexpr value_type get( int x, int y ) const { return *( line( y ) + ( x - _x1 ) ); }

private:
	const float *_ptr = nullptr;
	int _x1 = 0;
	int _y1 = 0;
	int _x2 = 0;
	int _y2 = 0;
	int _stride = 0;
};

} // namespace image

