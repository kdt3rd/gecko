// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "versor.h"

#include <cmath>

namespace gl
{
////////////////////////////////////////

versor::versor( float angle, float x, float y, float z )
{
    _q[0] = std::cos( angle / 2.F );
    _q[1] = std::sin( angle / 2.F ) * x;
    _q[2] = std::sin( angle / 2.F ) * y;
    _q[3] = std::sin( angle / 2.F ) * z;
}

////////////////////////////////////////

void versor::normalize( void )
{
    float sum = _q[0] * _q[0] + _q[1] * _q[1] + _q[2] * _q[2] + _q[3] * _q[3];
    const float thresh = 0.0001F;
    if ( std::abs( 1.F - sum ) >= thresh )
    {
        float mag = std::sqrt( sum );
        _q[0] /= mag;
        _q[1] /= mag;
        _q[2] /= mag;
        _q[3] /= mag;
    }
}

////////////////////////////////////////

versor versor::slerp( const versor &v1, const versor &v2, float t )
{
    // Angle between the 2 versor (cos of half angle).
    float cos_angle = v1 ^ v2;

    // Negative angle, go the short way around instead.
    if ( cos_angle < 0.F )
        return slerp( -v1, v2, t );

    // If angle between the versor is 0, don't bother interpolating.
    if ( cos_angle >= 1.F )
        return v1;

    float sin_angle = std::sqrt( 1.F - cos_angle * cos_angle );

    if ( std::abs( sin_angle ) < 0.0001F )
        return v1 * ( 1.F - t ) + v2 * t;

    float half_angle = std::acos( cos_angle );
    float a          = std::sin( ( 1.F - t ) * half_angle ) / sin_angle;
    float b          = std::sin( t * half_angle ) / sin_angle;

    return v1 * a + v2 * b;
}

////////////////////////////////////////

} // namespace gl
