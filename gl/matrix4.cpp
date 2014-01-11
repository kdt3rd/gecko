
#include "matrix4.h"
#include <cstddef>

namespace gl
{

////////////////////////////////////////

matrix4::matrix4( void )
{
	for ( size_t i = 0; i < 16; ++i )
		_data[i] = 0.F;
}

////////////////////////////////////////

void matrix4::identity( void )
{
	_data[0] = 1.F;
	_data[1] = 0.F;
	_data[2] = 0.F;
	_data[3] = 0.F;
	_data[4] = 0.F;
	_data[5] = 1.F;
	_data[6] = 0.F;
	_data[7] = 0.F;
	_data[8] = 0.F;
	_data[9] = 0.F;
	_data[10] = 1.F;
	_data[11] = 0.F;
	_data[12] = 0.F;
	_data[13] = 0.F;
	_data[14] = 0.F;
	_data[15] = 1.F;
}

////////////////////////////////////////

void matrix4::ortho( float left, float right, float bottom, float top )
{
	_data[0] = 2.F / ( right - left );
	_data[1] = 0.F;
	_data[2] = 0.F;
	_data[3] = 0.F;
	_data[4] = 0.F;
	_data[5] = 2.F / ( top - bottom );
	_data[6] = 0.F;
	_data[7] = 0.F;
	_data[8] = 0.F;
	_data[9] = 0.F;
	_data[10] = -1.F;
	_data[11] = 0.F;
	_data[12] = - ( right + left ) / ( right - left );
	_data[13] = - ( top + bottom ) / ( top - bottom );
	_data[14] = 0.F;
	_data[15] = 1.F;
}

////////////////////////////////////////

void matrix4::scale( float x, float y, float z )
{
	_data[0] *= x;
	_data[5] *= y;
	_data[10] *= z;
}

////////////////////////////////////////

void matrix4::translate( float x, float y, float z )
{
	_data[12] += x;
	_data[13] += y;
	_data[14] += z;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const matrix4 &m )
{
	const float *data = m.data();
	for ( size_t i = 0; i < 4; ++i )
	{
		out << *data++ << ' ';
		out << *data++ << ' ';
		out << *data++ << ' ';
		out << *data++ << '\n';
	}

	return out;
}

////////////////////////////////////////

}

