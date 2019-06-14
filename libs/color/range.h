// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "space.h"

#include <base/contract.h>
#include <cmath>
#include <iostream>

////////////////////////////////////////

namespace color
{
/// @brief Describes the normalization of encoded values
///
/// This covers all the variants of the classic full vs. legal
enum class range : uint8_t
{
    FULL,     ///< normal 0 - 2^bits - 1 integer encoding 0.0 - 1.0
    ITU_FULL, ///< 0 - 2^bits ITU-BT.2100 defined scaling w/ a clamp at 1023.0/1024.0 (so 4092 for 12-bit) (GRRRRRRRRR)
    SMPTE,    ///< also called legal or video range
              ///< for luma: 16-235 (scaled based on bits)
              ///< for chroma: 16-240 (scaled based on bits)
    ///< reserved (illegal) values: 0,255 for 8 bits, 0-3,1020-1023 for 10, etc.
    SDI, ///< same reserved values as SMPTE, but
         ///< scales the non-reserved values 0-1 with
         ///< no extra gap range
    SDI_RP2077, ///< same reserved values as SMPTE and SDI, but no scaling (per SMPTE RP 2077)
    SDI_ST2084, ///< same reserved values as SMPTE and SDI, but scaling per SMPTE 2084
    SMPTE_PLUS ///< A combination of SMPTE and SIGNAL
               ///< where the black level is the same
               ///< as legal, but the peak is the same as signal
};

template <typename T>
void to_full(
    T &     outA,
    T &     outB,
    T &     outC,
    const T inA,
    const T inB,
    const T inC,
    const space /*s*/,
    const range r,
    const int /*bits*/ )
{
    switch ( r )
    {
        case range::FULL:
            outA = inA;
            outB = inB;
            outC = inC;
            break;
        case range::ITU_FULL:
        {
            static const T inScale = T( 1023.0 / 1024.0 );
            outA                   = inA * inScale;
            outB                   = inB * inScale;
            outC                   = inC * inScale;
            break;
        }
        case range::SMPTE: throw_not_yet();
        case range::SDI: throw_not_yet();
        case range::SDI_RP2077:
            outA = inA;
            outB = inB;
            outC = inC;
            break;
        case range::SDI_ST2084: throw_not_yet();
        case range::SMPTE_PLUS: throw_not_yet();
    }
}

template <class T, class Enable = void> struct sdi_util
{
    static inline void clamp_illegal( T &a, T &b, T &c, const int bits )
    {
        if ( bits <= 8 )
        {
            static const T minV = T( 1.0 / 255.0 );
            static const T maxV = T( 254.0 / 255.0 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
        else if ( bits <= 10 )
        {
            static const T minV = T( 4.0 / 1023.0 );
            static const T maxV = T( 1019.0 / 1023.0 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
        else if ( bits <= 12 )
        {
            static const T minV = T( 16.0 / 4095.0 );
            static const T maxV = T( 4079.0 / 4095.0 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
    }
};

template <class T>
struct sdi_util<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    // TODO: can we special case based on the type of T?
    static inline void clamp_illegal( T &a, T &b, T &c, const int bits )
    {
        if ( bits <= 8 )
        {
            static const T minV = T( 1 );
            static const T maxV = T( 254 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
        else if ( bits <= 10 )
        {
            static const T minV = T( 4 );
            static const T maxV = T( 1019 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
        else if ( bits <= 12 )
        {
            static const T minV = T( 16 );
            static const T maxV = T( 4079 );
            a                   = std::min( maxV, std::max( minV, a ) );
            b                   = std::min( maxV, std::max( minV, b ) );
            c                   = std::min( maxV, std::max( minV, c ) );
        }
    }
};

/// assumes values in ~0-1
/// clamping only applies to other ranges besides FULL?
template <typename T>
void from_full(
    T &     outA,
    T &     outB,
    T &     outC,
    const T inA,
    const T inB,
    const T inC,
    space /*s*/,
    range r,
    int   bits,
    bool  doClampIllegal )
{
    switch ( r )
    {
        case range::FULL:
            outA = inA;
            outB = inB;
            outC = inC;
            /// never clamps?
            return;

        case range::ITU_FULL:
        {
            static const T outScale = T( 1024.0 / 1023.0 );
            outA                    = inA * outScale;
            outB                    = inB * outScale;
            outC                    = inC * outScale;
            if ( doClampIllegal )
            {
                static const T outMax = T( 1023.0 / 1024.0 );
                outA = std::max( T( 0 ), std::min( outA, outMax ) );
                outB = std::max( T( 0 ), std::min( outB, outMax ) );
                outC = std::max( T( 0 ), std::min( outC, outMax ) );
            }
            break;
        }
        case range::SMPTE: throw_not_yet(); break;
        case range::SDI: throw_not_yet(); break;
        case range::SDI_RP2077:
            outA = inA;
            outB = inB;
            outC = inC;
            break;
        case range::SDI_ST2084: throw_not_yet(); break;
        case range::SMPTE_PLUS: throw_not_yet(); break;
    }

    if ( doClampIllegal )
        sdi_util<T>::clamp_illegal( outA, outB, outC, bits );
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const range &r )
{
    switch ( r )
    {
        case range::FULL: os << "full"; break;
        case range::ITU_FULL: os << "itu_full"; break;
        case range::SMPTE: os << "smpte"; break;
        case range::SDI: os << "sdi"; break;
        case range::SDI_RP2077: os << "sdi_rp2077"; break;
        case range::SDI_ST2084: os << "sdi_st2084"; break;
        case range::SMPTE_PLUS: os << "smpte_plus"; break;
    }
    return os;
}

} // namespace color
