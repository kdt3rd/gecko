
#include "matrix4.h"
#include <cstddef>

namespace gl
{

////////////////////////////////////////

matrix4 &matrix4::operator*=( const matrix4 &m )
{
	*this = *this * m;
	return *this;
}

////////////////////////////////////////

matrix4 matrix4::ortho( float left, float right, float top, float bottom )
{
	return matrix4
	{
		2.F / ( right - left ), 0.F, 0.F, 0.F,
		0.F, 2.F / ( top - bottom ), 0.F, 0.F,
		0.F, 0.F, -1.F, 0.F,
		- ( right + left ) / ( right - left ), - ( top + bottom ) / ( top - bottom ), 0.F, 1.F
	};
}

////////////////////////////////////////

matrix4 matrix4::scale( float x, float y, float z )
{
	return matrix4
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
}

////////////////////////////////////////

matrix4 matrix4::translation( float x, float y, float z )
{
	return matrix4
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
}

////////////////////////////////////////

matrix4 operator*( const matrix4 &a, const matrix4 &b )
{
	return matrix4
	{
		a.row0() * b.col0(),
		a.row0() * b.col1(),
		a.row0() * b.col2(),
		a.row0() * b.col3(),
		a.row1() * b.col0(),
		a.row1() * b.col1(),
		a.row1() * b.col2(),
		a.row1() * b.col3(),
		a.row2() * b.col0(),
		a.row2() * b.col1(),
		a.row2() * b.col2(),
		a.row2() * b.col3(),
		a.row3() * b.col0(),
		a.row3() * b.col1(),
		a.row3() * b.col2(),
		a.row3() * b.col3()
	};
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

