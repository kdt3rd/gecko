
#pragma once

#include <iostream>
#include "vector4.h"

namespace gl
{

////////////////////////////////////////

/// @brief A 4x4 float matrix
class matrix4
{
public:
	/// @brief Default constructor
	matrix4( void )
	{
	}

	/// @brief Constructor
	matrix4( std::initializer_list<float> l )
	{
		std::copy( l.begin(), l.end(), _data );
	}

	/// @brief Get float data
	const float *data( void ) const
	{
		return _data;
	}

	/// @brief Multiply by a matrix
	matrix4 &operator*=( const matrix4 &m );

	/// @brief Get row 0
	vector4 row0( void ) const
	{
		return { _data[0], _data[1], _data[2], _data[3] };
	}

	/// @brief Get row 1
	vector4 row1( void ) const
	{
		return { _data[4], _data[5], _data[6], _data[7] };
	}

	/// @brief Get row 2
	vector4 row2( void ) const
	{
		return { _data[8], _data[9], _data[10], _data[11] };
	}

	/// @brief Get row 3
	vector4 row3( void ) const
	{
		return { _data[12], _data[13], _data[14], _data[15] };
	}

	/// @brief Get column 0
	vector4 col0( void ) const
	{
		return { _data[0], _data[4], _data[8], _data[12] };
	}

	/// @brief Get column 1
	vector4 col1( void ) const
	{
		return { _data[1], _data[5], _data[9], _data[13] };
	}

	/// @brief Get column 2
	vector4 col2( void ) const
	{
		return { _data[2], _data[6], _data[10], _data[14] };
	}

	/// @brief Get column 3
	vector4 col3( void ) const
	{
		return { _data[3], _data[7], _data[11], _data[15] };
	}

	/// @brief Construct identity matrix
	static matrix4 identity( void );

	/// @brief Construct translation matrix
	static matrix4 translation( float x, float y, float z = 0.0 );

	/// @brief Construct scale matrix
	static matrix4 scale( float x, float y, float z = 1.0 );

	/// @brief Construct orthographic projection matrix
	static matrix4 ortho( float left, float right, float top, float bottom );

private:
	float _data[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};

////////////////////////////////////////

/// @brief Multiply 2 matrices
matrix4 operator*( const matrix4 &a, const matrix4 &b );

/// @brief Stream out a matrix
std::ostream &operator<<( std::ostream &out, const matrix4 &m );

}

