// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>
#include <sstream>
#include <string>

////////////////////////////////////////

namespace color
{
// when looking at the sYCC amendment, they define the negative value reflection
// of the gamma curve
// however, the original spec says to discard values above 1 / below 0...
template <typename T> struct gamma_srgb
{
    // TODO: make sure this allows SSE values
    static_assert(
        std::is_floating_point<T>::value, "Expecting a floating point type" );

    inline std::string to_linear_glsl( const std::string &funcName )
    {
        // TODO: uniforms and parameters?
        std::stringstream func;
        func
            << "float " << funcName
            << "(float v)\n"
               "{\n"
               "    float x = fabs( v );\n"
               "    x = x <= 0.04045f ? x / 12.92f : pow( ( x + 0.055f ) / 1.055f, 2.4f );\n"
               "    return copysign( x, v );\n"
               "}\n";
        return func.str();
    }

    inline std::string from_linear_glsl( const std::string &funcName )
    {
        // TODO: uniforms and parameters?
        std::stringstream func;
        func
            << "float " << funcName
            << "(float v)\n"
               "{\n"
               "    float x = fabs( v );\n"
               "    x = x <= 0.0031308f ? 12.92f * x : pow( x, 1.f / 2.4f ) * 1.055f - 0.055f;\n"
               "    return copysign( x, v );\n"
               "}\n";
        return func.str();
    }

    constexpr inline T to_linear( const T v ) const
    {
        static_assert(
            std::is_floating_point<T>::value,
            "expect a floating point value for transfer curve to/from" );
        return std::abs( v ) <= T( 0.04045 )
                   ? v / T( 12.92 )
                   : std::copysign(
                         std::pow(
                             ( std::abs( v ) + T( 0.055 ) ) / T( 1.055 ),
                             T( 2.4 ) ),
                         v );
    }

    constexpr inline T from_linear( const T v ) const
    {
        static_assert(
            std::is_floating_point<T>::value,
            "expect a floating point value for transfer curve to/from" );
        return std::abs( v ) <= T( 0.0031308 )
                   ? v * T( 12.92 )
                   : std::copysign(
                         T( 1.055 ) *
                                 std::pow( std::abs( v ), T( 1.0 / 2.4 ) ) -
                             T( 0.055 ),
                         v );
    }

    static constexpr inline T linearize( const T v )
    {
        return gamma_srgb().to_linear( v );
    }

    static constexpr inline T encode( const T v )
    {
        return gamma_srgb().from_linear( v );
    }
};

} // namespace color
