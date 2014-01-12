
#pragma once

#include <iostream>
#include "vector4.h"

namespace gl
{

////////////////////////////////////////

class matrix4
{
public:
	matrix4( void )
	{
	}

	matrix4( std::initializer_list<float> l )
	{
		std::copy( l.begin(), l.end(), _data );
	}

	const float *data( void ) const
	{
		return _data;
	}

	matrix4 &operator*=( const matrix4 &m );

	vector4 row0( void ) const
	{
		return { _data[0], _data[1], _data[2], _data[3] };
	}

	vector4 row1( void ) const
	{
		return { _data[4], _data[5], _data[6], _data[7] };
	}

	vector4 row2( void ) const
	{
		return { _data[8], _data[9], _data[10], _data[11] };
	}

	vector4 row3( void ) const
	{
		return { _data[12], _data[13], _data[14], _data[15] };
	}

	vector4 col0( void ) const
	{
		return { _data[0], _data[4], _data[8], _data[12] };
	}

	vector4 col1( void ) const
	{
		return { _data[1], _data[5], _data[9], _data[13] };
	}

	vector4 col2( void ) const
	{
		return { _data[2], _data[6], _data[10], _data[14] };
	}

	vector4 col3( void ) const
	{
		return { _data[3], _data[7], _data[11], _data[15] };
	}

	static matrix4 identity( void );
	static matrix4 translation( float x, float y, float z = 0.0 );
	static matrix4 scale( float x, float y, float z = 1.0 );
	static matrix4 ortho( float left, float right, float top, float bottom );

private:
	float _data[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};

////////////////////////////////////////

matrix4 operator*( const matrix4 &a, const matrix4 &b );
std::ostream &operator<<( std::ostream &out, const matrix4 &m );

}

