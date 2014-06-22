
#pragma once

#include <initializer_list>

namespace gl
{

////////////////////////////////////////

class vector4
{
public:
	vector4( std::initializer_list<float> l )
	{
		std::copy( l.begin(), l.end(), _data );
	}

	const float *data( void ) const
	{
		return _data;
	}

private:
	float _data[4];
};

////////////////////////////////////////

inline float operator*( const vector4 &va, const vector4 &vb )
{
	const float *a = va.data();
	const float *b = vb.data();
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

////////////////////////////////////////

}

