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
#include <engine/computed_value.h>
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

class accum_buf : public engine::computed_base
{
public:
	accum_buf( void );
	accum_buf( int w, int h );
	template <typename... Args>
	inline accum_buf( const base::cstring &opname, const engine::dimensions &d, Args &&... args )
			: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... ),
			  _width( static_cast<int>( d.x ) ),
			  _height( static_cast<int>( d.y ) )
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
		r.x = static_cast<engine::dimensions::value_type>( width() );
		r.y = static_cast<engine::dimensions::value_type>( height() );
		return r;
	}

	inline int width( void ) const { return _width; }
	inline int height( void ) const { return _height; }

	inline int stride( void ) const { check_compute(); return _stride; }

	inline size_t buffer_size( void ) const { return static_cast<size_t>( stride() * height() ) * sizeof(double); }

	inline double *data( void ) { check_compute(); return _mem.get(); }
	inline const double *data( void ) const { check_compute(); return _mem.get(); }
	inline const double *cdata( void ) const { check_compute(); return _mem.get(); }
	inline double *line( int y ) { return data() + y * stride(); }
	inline const double *line( int y ) const { return cdata() + y * stride(); }

	inline double &get( int x, int y ) { return *( line( y ) + x ); }
	inline double get( int x, int y ) const { return *( line( y ) + x ); }
	inline void set( int x, int y, double v ) { get( x, y ) = v; }
	template <typename T>
	inline void set( int x, int y, T v ) { get( x, y ) = static_cast<double>( v ); }

private:
	void check_compute( void ) const;

	mutable std::shared_ptr<double> _mem;
	int _width = 0;
	int _height = 0;
	mutable int _stride = 0;
};

engine::hash &operator<<( engine::hash &h, const accum_buf &p );

} // namespace image

