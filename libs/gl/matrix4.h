
#pragma once

#include <iostream>
#include "vector4.h"
#include <base/point.h>
#include <base/contract.h>

namespace gl
{

////////////////////////////////////////

/// @brief A 4x4 float matrix
class matrix4
{
public:
	/// @brief Default (identity) constructor
	matrix4( void )
	{
	}

	/// @brief Constructor
	matrix4( std::initializer_list<float> l )
	{
		// @TODO Use static_assert in C++14
		precondition( l.size() == 16, "expected 16 initializer values" );
		std::copy( l.begin(), l.end(), _data );
	}

	/// @brief Get float data
	const float *data( void ) const
	{
		return _data;
	}

	matrix4 inverted( void ) const;

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

	float get( size_t x, size_t y ) const
	{
		return _data[y*4+x];
	}

	float &get( size_t x, size_t y )
	{
		return _data[y*4+x];
	}

	void translate_x( float dx )
	{
		_data[12] += dx;
	}

	void translate_y( float dy )
	{
		_data[13] += dy;
	}

	void translate_z( float dz )
	{
		_data[14] += dz;
	}

	/// @brief Construct identity matrix
	static matrix4 identity( void );

	/// @brief Construct translation matrix
	static matrix4 translation( float x, float y, float z = 0.0 );
	static inline matrix4 translation( const base::point &p )
	{
		return translation( static_cast<float>( p.x() ),
							static_cast<float>( p.y() ) );
	}

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

