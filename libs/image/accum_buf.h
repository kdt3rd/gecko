// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include "allocator.h"
#include <engine/computed_value.h>
#include "op_registry.h"
#include "plane_util.h"

////////////////////////////////////////

namespace image
{

class accum_buf : public engine::computed_base
{
public:
	typedef double value_type;

	accum_buf( void );
	accum_buf( int x1, int y1, int x2, int y2 );
	template <typename... Args>
	inline accum_buf( const base::cstring &opname, const engine::dimensions &d, Args &&... args )
			: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... ),
			  _x1( d.x1 ), _y1( d.y1 ), _x2( d.x2 ), _y2( d.y2 )
	{
	}

	accum_buf( const accum_buf & );
	accum_buf( accum_buf && );
	accum_buf &operator=( const accum_buf & );
	accum_buf &operator=( accum_buf && );
	~accum_buf( void );

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r;
		r.x1 = static_cast<engine::dimensions::value_type>( _x1 );
		r.y1 = static_cast<engine::dimensions::value_type>( _y1 );
		r.x2 = static_cast<engine::dimensions::value_type>( _x2 );
		r.y2 = static_cast<engine::dimensions::value_type>( _y2 );
		r.planes = 1;
		r.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(double) );
		return r;
	}
	inline int x1( void ) const { return _x1; }
	inline int y1( void ) const { return _y1; }
	inline int x2( void ) const { return _x2; }
	inline int y2( void ) const { return _y2; }

	inline int width( void ) const { return ( _x2 - _x1 + 1 ); }
	inline int height( void ) const { return ( _y2 - _y1 + 1 ); }

	inline int stride( void ) const { check_compute(); return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(double); }

	inline double *data( void ) { check_compute(); return _mem.get(); }
	inline const double *data( void ) const { check_compute(); return _mem.get(); }
	inline const double *cdata( void ) const { check_compute(); return _mem.get(); }
	inline double *line( int y ) { return data() + ( y - _y1 ) * stride(); }
	inline const double *line( int y ) const { return cdata() + ( y - _y1 ) * stride(); }

	inline double &get( int x, int y ) { return *( line( y ) + ( x - _x1 ) ); }
	inline double get( int x, int y ) const { return *( line( y ) + ( x - _x1 ) ); }
	inline void set( int x, int y, double v ) { get( x, y ) = v; }
	template <typename T>
	inline void set( int x, int y, T v ) { get( x, y ) = static_cast<double>( v ); }

private:
	void check_compute( void ) const;

	mutable std::shared_ptr<double> _mem;
	int _x1 = 0;
	int _y1 = 0;
	int _x2 = 0;
	int _y2 = 0;
	mutable int _stride = 0;
};

engine::hash &operator<<( engine::hash &h, const accum_buf &p );

} // namespace image

