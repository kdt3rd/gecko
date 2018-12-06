//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include "plane.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

class vector_field : public engine::computed_base
{
public:
	vector_field( void );
	vector_field( int x1, int y1, int x2, int y2, bool isAbsolute );
	vector_field( const plane &u, const plane &v, bool isAbsolute );
	vector_field( plane &&u, plane &&v, bool isAbsolute );
	vector_field( const engine::dimensions &d, bool isAbsolute );

	template <typename... Args>
	inline vector_field( bool isAbsolute, const base::cstring &opname, const engine::dimensions &d, Args &&... args )
			: computed_base( image::op_registry(), opname, d, std::forward<Args>( args )... ),
			  _u( "v.extract_u", d, *this ), _v( "v.extract_v", d, *this ),
			  _absolute( isAbsolute )
	{
	}

	vector_field( const vector_field & );
	vector_field( vector_field && );
	vector_field &operator=( const vector_field & );
	vector_field &operator=( vector_field && );
	~vector_field( void );

	inline bool valid( void ) const { return _u.valid() && _v.valid(); }

	inline engine::dimensions dims( void ) const
	{
		engine::dimensions r = _u.dims();
		r.planes = 2;
		return r;
	}

	inline int x1( void ) const { return _u.x1(); }
	inline int y1( void ) const { return _u.y1(); }
	inline int x2( void ) const { return _u.x2(); }
	inline int y2( void ) const { return _u.y2(); }

	inline int width( void ) const { return _u.width(); }
	inline int height( void ) const { return _u.height(); }

	inline bool is_absolute( void ) const { return _absolute; }
	inline plane &u( void ) { return _u; }
	inline const plane &u( void ) const { return _u; }
	inline plane &v( void ) { return _v; }
	inline const plane &v( void ) const { return _v; }
	
	static vector_field create( const plane &a, const plane &b, bool isAbsolute );
	static vector_field create( plane &&a, plane &&b, bool isAbsolute );

private:
	plane _u;
	plane _v;

	bool _absolute = false;
};

engine::hash &operator<<( engine::hash &h, const vector_field &v );

} // namespace image

