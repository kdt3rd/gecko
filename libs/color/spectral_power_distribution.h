// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "chroma_coord.h"
#include "spectral_density.h"

#include <array>
#include <cmath>
#include <type_traits>

////////////////////////////////////////

namespace color
{
template <typename T> class blackbody_SPD
{
public:
    using value_type              = T;
    static constexpr value_type c = 2.99792458e8;      // velocity of light m/s
    static constexpr value_type h = T( 6.626176e-34 ); // Planck constant Js
    static constexpr value_type k =
        T( 1.380662e-23 ); // boltzmann constant J / K
    static constexpr value_type c1 = T( 2.0 * M_PI * h * c * c );
    static constexpr value_type c2 = T( h * c / k );

    explicit constexpr blackbody_SPD( value_type tK ) noexcept : _kelvin( tK )
    {}
    blackbody_SPD( void )                                     = delete;
    constexpr blackbody_SPD( const blackbody_SPD & ) noexcept = default;
    blackbody_SPD &operator=( const blackbody_SPD & ) noexcept = default;
    constexpr blackbody_SPD( blackbody_SPD && ) noexcept       = default;
    blackbody_SPD &operator=( blackbody_SPD && ) noexcept = default;
    ~blackbody_SPD( void )                                = default;

    inline value_type operator()( value_type lambda ) const
    {
        return sample( lambda );
    }
    inline value_type sample( value_type l ) const
    {
        return c1 / ( ( l * l * l * l * l ) *
                      ( std::exp( c2 / ( _kelvin * l ) ) - 1 ) );
    }
};

// computes in relative form, such that
template <typename T> class illuminant_D_SPD
{
public:
    using value_type = T;
    using table      = spectral_density<value_type>;

    template <typename O>
    explicit constexpr illuminant_D_SPD( const chroma_coord<O> &cx ) noexcept
        : _m( value_type(
              O( 0.0241 ) + O( 0.2562 ) * cx.x - O( 0.7341 ) * cx.y ) )
        , _m1( value_type(
                   O( -1.3515 ) - O( 1.7703 ) * cx.x + O( 5.9114 ) * cx.y ) /
               _m )
        , _m2( value_type(
                   O( 0.0300 ) - O( 31.4424 ) * cx.x + O( 30.0717 ) * cx.y ) /
               _m )
        ,
    {}
    illuminant_D_SPD( void )                                        = delete;
    constexpr illuminant_D_SPD( const illuminant_D_SPD & ) noexcept = default;
    illuminant_D_SPD &operator=( const illuminant_D_SPD & ) noexcept = default;
    constexpr illuminant_D_SPD( illuminant_D_SPD && ) noexcept       = default;
    illuminant_D_SPD &operator=( illuminant_D_SPD && ) noexcept = default;
    ~illuminant_D_SPD( void )                                   = default;

    inline value_type operator()( value_type lambda ) const
    {
        return sample( lambda );
    }
    inline value_type sample( value_type lambda ) const
    {
        return _s0( lambda ) + _m1 * _s1( lambda ) + _m2 * _s1( lambda );
    }

private:
    // from Lindbloom DIlluminant.xls
    static const table _s0 = {
        300.0, 10.0, { 0.04,   6.00,   29.60,  55.30,  57.30,  61.80,  61.50,
                       68.80,  63.40,  65.80,  94.80,  104.80, 105.90, 96.80,
                       113.90, 125.60, 125.50, 121.30, 121.30, 113.50, 113.10,
                       110.80, 106.50, 108.80, 105.30, 104.40, 100.00, 96.00,
                       95.10,  89.10,  90.50,  90.30,  88.40,  84.00,  85.10,
                       81.90,  82.60,  84.90,  81.30,  71.90,  74.30,  76.40,
                       63.30,  71.70,  77.00,  65.20,  47.70,  68.60,  65.00,
                       66.00,  61.00,  53.30,  58.90,  61.90 }
    };
    static const table _s1 = {
        300.0, 10.0, { 0.02,   4.50,   22.40,  42.00,  40.60,  41.60,  38.00,
                       42.40,  38.50,  35.00,  43.40,  46.30,  43.90,  37.10,
                       36.70,  35.90,  32.60,  27.90,  24.30,  20.10,  16.20,
                       13.20,  8.60,   6.10,   4.20,   1.90,   0.00,   -1.60,
                       -3.50,  -3.50,  -5.80,  -7.20,  -8.60,  -9.50,  -10.90,
                       -10.70, -12.00, -14.00, -13.60, -12.00, -13.30, -12.90,
                       -10.60, -11.60, -12.20, -10.20, -7.80,  -11.20, -10.40,
                       -10.60, -9.70,  -8.30,  -9.30,  -9.80 }
    };
    static const table _s2 = {
        300.0, 10.0, { 0.0,  2.0,  4.0,  8.5,  7.8,  6.7,  5.3,  6.1,  3.0,
                       1.2,  -1.1, -0.5, -0.7, -1.2, -2.6, -2.9, -2.8, -2.6,
                       -2.6, -1.8, -1.5, -1.3, -1.2, -1.0, -0.5, -0.3, 0.0,
                       0.2,  0.5,  2.1,  3.2,  4.1,  4.7,  5.1,  6.7,  7.3,
                       8.6,  9.8,  10.2, 8.3,  9.6,  8.5,  7.0,  7.6,  8.0,
                       6.7,  5.2,  7.4,  6.8,  7.0,  6.4,  5.5,  6.1,  6.5 }
    };

    const value_type _m;
    const value_type _m1;
    const value_type _m2;
};

} // namespace color
