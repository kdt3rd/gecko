// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "chroma_coord.h"
#include "triplet.h"

#include <base/contract.h>
#include <base/math_functions.h>

#include <cmath>
#include <stdexcept>

////////////////////////////////////////

namespace color
{
enum class standard_illuminant
{
    A,
    B,
    C,
    D50,
    D55,
    D60,
    D65,
    D75,
    DCI_PROJ,
    E,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
};

/// CIE 1931 two degree observer numbers
namespace two_deg
{
template <typename T>
inline chroma_coord<T> illuminant( standard_illuminant si )
{
    switch ( si )
    {
        case standard_illuminant::A:
            return chroma_coord<T>( T( 0.44757 ), T( 0.40745 ) );
        case standard_illuminant::B:
            return chroma_coord<T>( T( 0.34842 ), T( 0.35161 ) );
        case standard_illuminant::C:
            return chroma_coord<T>( T( 0.31006 ), T( 0.31616 ) );
        case standard_illuminant::D50:
            return chroma_coord<T>( T( 0.34567 ), T( 0.35850 ) );
        case standard_illuminant::D55:
            return chroma_coord<T>( T( 0.33242 ), T( 0.34743 ) );
        case standard_illuminant::D60:
            return chroma_coord<T>( T( 0.32168 ), T( 0.33767 ) );
            // standards truncate this to 4 digits, but we'll specify
            // the white in the standard instead of using these
            // numbers, so leave these as the pure definition
        case standard_illuminant::D65:
            return chroma_coord<T>( T( 0.31271 ), T( 0.32902 ) );
        case standard_illuminant::D75:
            return chroma_coord<T>( T( 0.29902 ), T( 0.31485 ) );
        case standard_illuminant::DCI_PROJ:
            return chroma_coord<T>( T( 0.314 ), T( 0.351 ) );
        case standard_illuminant::E:
            return chroma_coord<T>( T( 1.0 / 3.0 ), T( 1.0 / 3.0 ) );
        case standard_illuminant::F1:
            return chroma_coord<T>( T( 0.31310 ), T( 0.33727 ) );
        case standard_illuminant::F2:
            return chroma_coord<T>( T( 0.37208 ), T( 0.37529 ) );
        case standard_illuminant::F3:
            return chroma_coord<T>( T( 0.40910 ), T( 0.39430 ) );
        case standard_illuminant::F4:
            return chroma_coord<T>( T( 0.44018 ), T( 0.40329 ) );
        case standard_illuminant::F5:
            return chroma_coord<T>( T( 0.31379 ), T( 0.34531 ) );
        case standard_illuminant::F6:
            return chroma_coord<T>( T( 0.37790 ), T( 0.38835 ) );
        case standard_illuminant::F7:
            return chroma_coord<T>( T( 0.31292 ), T( 0.32933 ) );
        case standard_illuminant::F8:
            return chroma_coord<T>( T( 0.34588 ), T( 0.35875 ) );
        case standard_illuminant::F9:
            return chroma_coord<T>( T( 0.37417 ), T( 0.37281 ) );
        case standard_illuminant::F10:
            return chroma_coord<T>( T( 0.34609 ), T( 0.35986 ) );
        case standard_illuminant::F11:
            return chroma_coord<T>( T( 0.38052 ), T( 0.37713 ) );
        case standard_illuminant::F12:
            return chroma_coord<T>( ( 0.43695 ), T( 0.40441 ) );
    }
    throw_runtime( "Unhandled standard illuminant" );
}

template <typename T> inline chroma_coord<T> illuminant_D( T ccct )
{
    double xd;
    if ( ccct < T( 4000 ) || ccct > T( 25000 ) )
        throw std::out_of_range( "Invalid CCCT for illuminant D" );
    if ( ccct <= T( 7000 ) )
        xd = -4.6070e9 / ( ccct * ccct * ccct ) + 2.9678e6 / ( ccct * ccct ) +
             0.09911e3 / ccct + 0.244063;
    else
        xd = -2.0064e9 / ( ccct * ccct * ccct ) + 1.9018e6 / ( ccct * ccct ) +
             0.24748e3 / ccct + 0.237040;
    double yd = -3.0 * xd * xd + 2.870 * xd - 0.275;
    return chroma_coord<T>( T( xd ), T( yd ) );
}

template <typename T> inline T CCCT( const triplet<T> &xyz )
{
    double ret = -1.0;
    if ( xyz.x > T( 0 ) && xyz.y > T( 0 ) && xyz.z > T( 0 ) )
    {
        static double recipT[] = { DBL_MIN,  10.0e-6,  20.0e-6,  30.0e-6,
                                   40.0e-6,  50.0e-6,  60.0e-6,  70.0e-6,
                                   80.0e-6,  90.0e-6,  100.0e-6, 125.0e-6,
                                   150.0e-6, 175.0e-6, 200.0e-6, 225.0e-6,
                                   250.0e-6, 275.0e-6, 300.0e-6, 325.0e-6,
                                   350.0e-6, 375.0e-6, 400.0e-6, 425.0e-6,
                                   450.0e-6, 475.0e-6, 500.0e-6, 525.0e-6,
                                   550.0e-6, 575.0e-6, 600.0e-6 };
        struct UVT
        {
            double u, v, t;
        };
        static struct UVT uvt[] = {
            { 0.18006, 0.26352, -0.24341 },
            { 0.18066, 0.26589, -0.25479 },
            { 0.18133, 0.26846, -0.26876 },
            { 0.18208, 0.27119, -0.28539 },
            { 0.18293, 0.27407, -0.30470 },
            { 0.18388, 0.27709, -0.32675 },
            { 0.18494, 0.28021, -0.35156 },
            { 0.18611, 0.28342, -0.37915 },
            { 0.18740, 0.28668, -0.40955 },
            { 0.18880, 0.28997, -0.44278 },
            { 0.19032, 0.29326, -0.47888 },
            { 0.19462, 0.30141, -0.58204 },
            { 0.19962, 0.30921, -0.70471 },
            { 0.20525, 0.31647, -0.84901 },
            { 0.21142, 0.32312, -1.0182 },
            { 0.21807, 0.32909, -1.2168 },
            { 0.22511, 0.33439, -1.4512 },
            { 0.23247, 0.33904, -1.7298 },
            { 0.24010, 0.34308, -2.0637 },
            { 0.24792,
              0.34655,
              -2.4681 }, /* Note: 0.24792 is a corrected value for the error found in W&S as 0.24702 */
            { 0.25591, 0.34951, -2.9641 },
            { 0.26400, 0.35200, -3.5814 },
            { 0.27218, 0.35407, -4.3633 },
            { 0.28039, 0.35577, -5.3762 },
            { 0.28863, 0.35714, -6.7262 },
            { 0.29685, 0.35823, -8.5955 },
            { 0.30505, 0.35907, -11.324 },
            { 0.31320, 0.35968, -15.628 },
            { 0.32129, 0.36011, -23.325 },
            { 0.32931, 0.36038, -40.770 },
            { 0.33724, 0.36051, -116.45 }
        };

        static const int nUVT = sizeof( uvt ) / sizeof( UVT );
        double us = ( 4.0 * xyz.x ) / ( xyz.x + 15.0 * xyz.y + 3.0 * xyz.z );
        double vs = ( 6.0 * xyz.y ) / ( xyz.x + 15.0 * xyz.y + 3.0 * xyz.z );
        double di, dm = 0.0;
        int    i;
        for ( i = 0; i < nUVT; ++i )
        {
            di = ( vs - uvt[i].v ) - uvt[i].t * ( us - uvt[i].u );
            if ( ( i > 0 ) && ( ( ( di < 0.0 ) && ( dm >= 0.0 ) ) ||
                                ( ( di >= 0.0 ) && ( dm < 0.0 ) ) ) )
                break;
            dm = di;
        }
        if ( i < nUVT )
        {
            di  = di / std::sqrt( 1.0 + uvt[i].t * uvt[i].t );
            dm  = dm / std::sqrt( 1.0 + uvt[i - 1].t * uvt[i - 1].t );
            ret = 1.0 /
                  base::lerp_fast( recipT[i - 1], recipT[i], dm / ( dm - di ) );
        }
    }
    return T( ret );
}

} // namespace two_deg

/// CIE 1964 10 degree observer
namespace ten_deg
{
template <typename T>
inline chroma_coord<T> illuminant( standard_illuminant si )
{
    switch ( si )
    {
        case standard_illuminant::A:
            return chroma_coord<T>( T( 0.45117 ), T( 0.40594 ) );
        case standard_illuminant::B:
            return chroma_coord<T>( T( 0.34980 ), T( 0.35270 ) );
        case standard_illuminant::C:
            return chroma_coord<T>( T( 0.31039 ), T( 0.31905 ) );
        case standard_illuminant::D50:
            return chroma_coord<T>( T( 0.34773 ), T( 0.35952 ) );
        case standard_illuminant::D55:
            return chroma_coord<T>( T( 0.33411 ), T( 0.34877 ) );
        case standard_illuminant::D60:
            return chroma_coord<T>(
                T( 0.322957407931312 ), T( 0.339135835524579 ) );
        case standard_illuminant::D65:
            return chroma_coord<T>( T( 0.31382 ), T( 0.33100 ) );
        case standard_illuminant::D75:
            return chroma_coord<T>( T( 0.29968 ), T( 0.31740 ) );
            // what do we put here?
            //		case standard_illuminant::DCI_PROJ: return chroma_coord<T>( T(0.314), T(0.351) );
        case standard_illuminant::DCI_PROJ: break;
        case standard_illuminant::E:
            return chroma_coord<T>( T( 1.0 / 3.0 ), T( 1.0 / 3.0 ) );
        case standard_illuminant::F1:
            return chroma_coord<T>( T( 0.31811 ), T( 0.33559 ) );
        case standard_illuminant::F2:
            return chroma_coord<T>( T( 0.37925 ), T( 0.36733 ) );
        case standard_illuminant::F3:
            return chroma_coord<T>( T( 0.41761 ), T( 0.38324 ) );
        case standard_illuminant::F4:
            return chroma_coord<T>( T( 0.44920 ), T( 0.39074 ) );
        case standard_illuminant::F5:
            return chroma_coord<T>( T( 0.31975 ), T( 0.34246 ) );
        case standard_illuminant::F6:
            return chroma_coord<T>( T( 0.38660 ), T( 0.37847 ) );
        case standard_illuminant::F7:
            return chroma_coord<T>( T( 0.31569 ), T( 0.32960 ) );
        case standard_illuminant::F8:
            return chroma_coord<T>( T( 0.34902 ), T( 0.35939 ) );
        case standard_illuminant::F9:
            return chroma_coord<T>( T( 0.37829 ), T( 0.37045 ) );
        case standard_illuminant::F10:
            return chroma_coord<T>( T( 0.35090 ), T( 0.35444 ) );
        case standard_illuminant::F11:
            return chroma_coord<T>( T( 0.38541 ), T( 0.37123 ) );
        case standard_illuminant::F12:
            return chroma_coord<T>( ( 0.44256 ), T( 0.39717 ) );
    }
    throw_runtime( "Unhandled standard illuminant" );
}

} // namespace ten_deg

} // namespace color
