
#pragma once

#include <iostream>

namespace gl
{

////////////////////////////////////////

class matrix4
{
public:
	matrix4( void );

	void identity( void );

	void ortho( float left, float right, float bottom, float top );

	void scale( float x, float y, float z = 1.0 );

	void translate( float dx, float dy, float dz = 0.0 );

	const float *data( void ) const
	{
		return _data;
	}

private:
	float _data[16];
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const matrix4 &m );

}

