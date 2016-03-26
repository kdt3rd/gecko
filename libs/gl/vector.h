
#pragma once

#include <base/contract.h>
#include <initializer_list>
#include <cmath>

namespace gl
{

////////////////////////////////////////

/// @brief A vector of 4 floats
template<size_t N>
class vector
{
public:
	static_assert( N > 0, "invalid vector size" );

	vector( void )
	{
	}

	/// @brief Constructor
	vector( const std::array<float,N> &l )
	{
		std::copy( l.begin(), l.end(), _data );
	}

	vector( const std::initializer_list<float> &l )
	{
		precondition( l.size() == N, "invalid vector intializer size" );
		std::copy( l.begin(), l.end(), _data );
	}

	template<typename ...Args>
	vector( Args ...args )
	{
		static_assert( sizeof...(Args) == N, "invalid initializer list" );
		setN( 0, args... );
	}

	float operator[]( size_t i ) const
	{
		return _data[i];
	}

	float &operator[]( size_t i )
	{
		return _data[i];
	}

	float at( size_t i ) const
	{
		precondition( i < N, "invalid index" );
		return _data[i];
	}

	/// @brief Get data
	const float *data( void ) const
	{
		return _data;
	}

	/// @brief Dot product of two vectors.
	inline float operator*( const vector<N> &b ) const
	{
		const vector<N> &a = *this;
		float result = a[0] * b[0];
		for ( size_t i = 1; i < N; ++i )
			result += a[i] * b[i];
		return result;
	}

	/// @brief Multiply vector.
	inline vector<N> operator*( float x ) const
	{
		vector<N> result;
		for ( size_t i = 0; i < N; ++i )
			result[i] = _data[i] * x;
		return result;
	}


	/// @brief Sum of two vectors.
	inline vector<N> operator+( const vector<N> &b ) const
	{
		vector<N> result;
		for ( size_t i = 0; i < N; ++i )
			result[i] = _data[i] + b[i];
		return result;
	}

	/// @brief Difference of 2 vectors
	inline vector<N> operator-( const vector<N> &b ) const
	{
		vector<N> result;
		for ( size_t i = 0; i < N; ++i )
			result[i] = _data[i] - b[i];
		return result;
	}

	static float distance_squared( const vector &a, const vector &b )
	{
		float d = a[0] - b[0];
		float dist = d * d;
		for ( size_t i = 1; i < N; ++i )
		{
			d = a[i] - b[i];
			dist += d * d;
		}
		return dist;
	}

	/// @brief Create a point using polar coordinates.
	/// @param r distance of the point from the origin (radius).
	/// @param a angle of the point, in radians.
	template<typename F1, typename F2>
	static vector<2> polar( F1 r, F2 a )
	{
		static_assert( std::is_floating_point<F1>::value, "polar requires floating point type" );
		static_assert( std::is_floating_point<F2>::value, "polar requires floating point type" );
		return { static_cast<float>(r) * static_cast<float>( std::cos( a ) ), static_cast<float>(r) * static_cast<float>( std::sin( a ) ) };
	}

private:
	void setN( size_t )
	{
		return;
	}

	template<typename ...Args>
	void setN( size_t n, float f, Args ...args )
	{
		_data[n] = f;
		setN( n + 1, args... );
	}

	float _data[N] = { 0 };
};

typedef vector<2> vec2;
typedef vector<3> vec3;
typedef vector<4> vec4;

/// @brief Write out a vector.
/// @relates gl::vector
template<size_t N>
std::ostream &operator<<( std::ostream &out, const vector<N> &v )
{
	out << v[0];
	for ( size_t i = 1; i < N; ++i )
		out << "," << v[i];
	return out;
}

////////////////////////////////////////

}

