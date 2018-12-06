//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "matrix4.h"
#include <cstddef>
#include <cmath>
#include <base/contract.h>

namespace gl
{

////////////////////////////////////////

matrix4 matrix4::inverted( void ) const
{
    float s0 = get( 0, 0 ) * get( 1, 1 ) - get( 1, 0 ) * get( 0, 1 );
    float s1 = get( 0, 0 ) * get( 1, 2 ) - get( 1, 0 ) * get( 0, 2 );
    float s2 = get( 0, 0 ) * get( 1, 3 ) - get( 1, 0 ) * get( 0, 3 );
    float s3 = get( 0, 1 ) * get( 1, 2 ) - get( 1, 1 ) * get( 0, 2 );
    float s4 = get( 0, 1 ) * get( 1, 3 ) - get( 1, 1 ) * get( 0, 3 );
    float s5 = get( 0, 2 ) * get( 1, 3 ) - get( 1, 2 ) * get( 0, 3 );

    float c5 = get( 2, 2 ) * get( 3, 3 ) - get( 3, 2 ) * get( 2, 3 );
    float c4 = get( 2, 1 ) * get( 3, 3 ) - get( 3, 1 ) * get( 2, 3 );
    float c3 = get( 2, 1 ) * get( 3, 2 ) - get( 3, 1 ) * get( 2, 2 );
    float c2 = get( 2, 0 ) * get( 3, 3 ) - get( 3, 0 ) * get( 2, 3 );
    float c1 = get( 2, 0 ) * get( 3, 2 ) - get( 3, 0 ) * get( 2, 2 );
    float c0 = get( 2, 0 ) * get( 3, 1 ) - get( 3, 0 ) * get( 2, 1 );

    // Should check for 0 determinant
    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    precondition( std::fabs( det ) > 0.00001F, "non-invertible matrix" );

    float invdet = 1.F / det;

    matrix4 result;

    result.get( 0, 0 ) = ( get( 1, 1 ) * c5 - get( 1, 2 ) * c4 + get( 1, 3 ) * c3 ) * invdet;
    result.get( 0, 1 ) = (-get( 0, 1 ) * c5 + get( 0, 2 ) * c4 - get( 0, 3 ) * c3 ) * invdet;
    result.get( 0, 2 ) = ( get( 3, 1 ) * s5 - get( 3, 2 ) * s4 + get( 3, 3 ) * s3 ) * invdet;
    result.get( 0, 3 ) = (-get( 2, 1 ) * s5 + get( 2, 2 ) * s4 - get( 2, 3 ) * s3 ) * invdet;

    result.get( 1, 0 ) = (-get( 1, 0 ) * c5 + get( 1, 2 ) * c2 - get( 1, 3 ) * c1 ) * invdet;
    result.get( 1, 1 ) = ( get( 0, 0 ) * c5 - get( 0, 2 ) * c2 + get( 0, 3 ) * c1 ) * invdet;
    result.get( 1, 2 ) = (-get( 3, 0 ) * s5 + get( 3, 2 ) * s2 - get( 3, 3 ) * s1 ) * invdet;
    result.get( 1, 3 ) = ( get( 2, 0 ) * s5 - get( 2, 2 ) * s2 + get( 2, 3 ) * s1 ) * invdet;

    result.get( 2, 0 ) = ( get( 1, 0 ) * c4 - get( 1, 1 ) * c2 + get( 1, 3 ) * c0 ) * invdet;
    result.get( 2, 1 ) = (-get( 0, 0 ) * c4 + get( 0, 1 ) * c2 - get( 0, 3 ) * c0 ) * invdet;
    result.get( 2, 2 ) = ( get( 3, 0 ) * s4 - get( 3, 1 ) * s2 + get( 3, 3 ) * s0 ) * invdet;
    result.get( 2, 3 ) = (-get( 2, 0 ) * s4 + get( 2, 1 ) * s2 - get( 2, 3 ) * s0 ) * invdet;

    result.get( 3, 0 ) = (-get( 1, 0 ) * c3 + get( 1, 1 ) * c1 - get( 1, 2 ) * c0 ) * invdet;
    result.get( 3, 1 ) = ( get( 0, 0 ) * c3 - get( 0, 1 ) * c1 + get( 0, 2 ) * c0 ) * invdet;
    result.get( 3, 2 ) = (-get( 3, 0 ) * s3 + get( 3, 1 ) * s1 - get( 3, 2 ) * s0 ) * invdet;
    result.get( 3, 3 ) = ( get( 2, 0 ) * s3 - get( 2, 1 ) * s1 + get( 2, 2 ) * s0 ) * invdet;

    return result;
}

////////////////////////////////////////

matrix4 &matrix4::operator*=( const matrix4 &m )
{
    *this = *this * m;
    return *this;
}

////////////////////////////////////////

matrix4 matrix4::zero( void )
{
    return matrix4
    {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
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

matrix4 matrix4::scaling( float x, float y, float z )
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

matrix4 matrix4::rotation( const versor &v )
{
    float w = v[0];
    float x = v[1];
    float y = v[2];
    float z = v[3];

    return matrix4
    {
    1 - 2 * y * y - 2 * z * z,
    2 * x * y + 2 * w * z,
    2 * x * z - 2 * w * y,
    0,
    2 * x * y - 2 * w * z,
    1 - 2 * x * x - 2 * z * z,
    2 * y * z + 2 * w * x,
    0,
    2 * x * z + 2 * w * y,
    2 * y * z - 2 * w * x,
    1 - 2 * x * x - 2 * y * y,
    0,
    0,
    0,
    0,
    1
    };
}

////////////////////////////////////////

matrix4 matrix4::ortho( float left, float right, float top, float bottom )
{
    float x = 2.F / ( right - left );
    float y = 2.F / ( top - bottom );
    float z = -1.F;
    float a = - ( right + left ) / ( right - left );
    float b = - ( top + bottom ) / ( top - bottom );

    return matrix4
    {
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    a, b, 0, 1
    };
}

////////////////////////////////////////

matrix4 matrix4::perspective( float vertical_fov, float aspect, float near, float far )
{
    precondition( vertical_fov > 0.F, "invalid vertical FOV {0}", vertical_fov );
    precondition( near < far, "invalide near/far distance ({0}/{1})", near, far );

    float range = std::tan( vertical_fov / 2.F ) * near;
    float x = ( 2.F * near ) / ( range * aspect + range * aspect );
    float y = near / range;
    float z = -( far + near ) / ( far - near );
    float p = -(2.0f * far * near) / (far - near);
    float n = -1.F;

    return matrix4
    {
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, n,
    0, 0, p, 0
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
