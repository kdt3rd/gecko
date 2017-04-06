//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
	inline plane_buffer( float *p, int w, int h, int wm1, int hm1, int s )
		: _ptr( p ), _w( w ), _h( h ), _wm1( wm1 ), _hm1( hm1 ), _stride( s )
	{}

	inline int width( void ) const { return _w; }
	inline int height( void ) const { return _h; }

	inline int lastx( void ) const { return _wm1; }
	inline int lasty( void ) const { return _hm1; }

	inline int stride( void ) const { return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline value_type *data( void ) { return _ptr; }
	inline const value_type *data( void ) const {  return _ptr; }
	inline const value_type *cdata( void ) const { return _ptr; }

	inline value_type *line( int y ) { return data() + y * stride(); }
	inline const value_type *line( int y ) const { return cdata() + y * stride(); }

	inline bool in_bounds_x( int x ) const { return x >= 0 && x < _w; }
	inline bool in_bounds_y( int y ) const { return y >= 0 && y < _h; }
	inline bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a reference to a value for setting purposes
	/// @sa plane_util.h for other access methods
	inline value_type &get( int x, int y ) { return *( line( y ) + x ); }
	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline value_type get( int x, int y ) const { return *( line( y ) + x ); }
	/// Alternate explicit set method
	inline void set( int x, int y, value_type v ) { get( x, y ) = v; }

private:
	float *_ptr = nullptr;
	int _w = 0;
	int _h = 0;
	int _wm1 = 0;
	int _hm1 = 0;
	int _stride = 0;
};


class const_plane_buffer
{
public:
	typedef float value_type;

	inline constexpr const_plane_buffer( void ) {}
	inline constexpr const_plane_buffer( const float *p, int w, int h, int wm1, int hm1, int s )
		: _ptr( p ), _w( w ), _h( h ), _wm1( wm1 ), _hm1( hm1 ), _stride( s )
	{}

	inline constexpr int width( void ) const { return _w; }
	inline constexpr int height( void ) const { return _h; }

	inline constexpr int lastx( void ) const { return _wm1; }
	inline constexpr int lasty( void ) const { return _hm1; }

	inline constexpr int stride( void ) const { return _stride; }

	inline constexpr size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(value_type); }

	inline constexpr const value_type *data( void ) const {  return _ptr; }
	inline constexpr const value_type *cdata( void ) const { return _ptr; }

	inline constexpr const value_type *line( int y ) const { return cdata() + y * stride(); }

	inline constexpr bool in_bounds_x( int x ) const { return x >= 0 && x < _w; }
	inline constexpr bool in_bounds_y( int y ) const { return y >= 0 && y < _h; }
	inline constexpr bool in_bounds( int x, int y ) const { return in_bounds_x( x ) && in_bounds_y( y ); }

	/// Get a value from the plane
	/// @sa plane_util.h for other access methods
	inline constexpr value_type get( int x, int y ) const { return *( line( y ) + x ); }

private:
	const float *_ptr = nullptr;
	int _w = 0;
	int _h = 0;
	int _wm1 = 0;
	int _hm1 = 0;
	int _stride = 0;
};

} // namespace image

