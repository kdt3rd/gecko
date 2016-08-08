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

#include "plane.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

class vector_field : public engine::computed_base
{
public:
	vector_field( void );
	vector_field( int w, int h, bool isAbsolute );
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
		engine::dimensions r;
		r.x = static_cast<engine::dimensions::value_type>( width() );
		r.y = static_cast<engine::dimensions::value_type>( height() );
		return r;
	}

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

