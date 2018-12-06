//
// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <array>
#include <initializer_list>

namespace gl
{

////////////////////////////////////////

class versor
{
public:
	/// @brief Construct from angle around the provided axis.
	versor( float angle, float x, float y, float z );

	versor( const std::array<float,4> &list )
		: _q( list )
	{
	}

	/// @brief Normalize
	void normalize( void );

	/// @brief Dot product.
	float operator^( const versor &v ) const
	{
		return _q[0] * v[0] + _q[1] * v[1] + _q[2] * v[2] + _q[3] * v[3];
	}

	versor operator*( const versor &v ) const
	{
		versor result {
			v[0] * _q[0] - v[1] * _q[1] - v[2] * _q[2] - v[3] * _q[3],
			v[0] * _q[1] + v[1] * _q[0] - v[2] * _q[3] + v[3] * _q[2],
			v[0] * _q[2] + v[1] * _q[3] + v[2] * _q[0] - v[3] * _q[1],
			v[0] * _q[3] - v[1] * _q[2] + v[2] * _q[1] + v[3] * _q[0]
		};
		result.normalize();
		return result;
	}

	versor operator*( float n ) const
	{
		return { _q[0] * n, _q[1] * n, _q[2] * n, _q[3] * n };
	}

	versor operator+( const versor &o ) const
	{
		versor result { _q[0] + o[0], _q[1] + o[1], _q[2] + o[2], _q[3] + o[3] };
		result.normalize();
		return result;
	}

	versor operator-( const versor &o ) const
	{
		versor result { _q[0] - o[0], _q[1] - o[1], _q[2] - o[2], _q[3] - o[3] };
		result.normalize();
		return result;
	}

	versor operator-( void ) const
	{
		return { -_q[0], -_q[1], -_q[2], -_q[3] };
	}

	/// @brief Access element at i
	float operator[]( size_t i ) const
	{
		return _q[i];
	}

	/// @brief Access element at i
	float &operator[]( size_t i )
	{
		return _q[i];
	}

	/// @brief Access element at i
	float at( size_t i ) const
	{
		return _q.at( i );
	}

	/// @brief Access element at i
	float &at( size_t i )
	{
		return _q.at( i );
	}

	static versor slerp( const versor &v1, const versor &v2, float t );

private:
	std::array<float,4> _q;
};

////////////////////////////////////////

}

