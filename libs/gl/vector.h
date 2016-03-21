
#pragma once

#include <base/contract.h>
#include <initializer_list>

namespace gl
{

////////////////////////////////////////

/// @brief A vector of 4 floats
template<size_t N>
class vector
{
public:
	static_assert( N > 0, "invalid vector size" );

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

	/// @brief Dot product of 2 vectors
	inline float operator*( const vector<N> &b ) const
	{
		const vector<N> &a = *this;
		float result = a[0] * b[0];
		for ( size_t i = 1; i < N; ++i )
			result += a[i] * b[i];
		return result;
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

	float _data[N];
};

typedef vector<2> vec2;
typedef vector<3> vec3;
typedef vector<4> vec4;

////////////////////////////////////////

}

