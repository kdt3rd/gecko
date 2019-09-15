// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "base64.h"

#include "contract.h"

namespace
{
const char charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr uint8_t WHITESPACE = 64;
constexpr uint8_t PADDING    = 65;
constexpr uint8_t INVALID    = 66;

static const uint8_t reverse[] = {
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 66, 66, 66, 65, 66, 66, 66, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66,
    66, 66, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66
};
} // namespace

namespace base
{
////////////////////////////////////////

std::string base64_encode( const void *message, size_t bytes )
{
    std::string result;
    result.reserve( ( ( bytes / 3 ) + ( bytes % 3 > 0 ) ) * 4 );

    uint32_t       temp;
    const uint8_t *cursor = static_cast<const uint8_t *>( message );
    for ( size_t i = 0; i < bytes / 3; ++i )
    {
        temp = static_cast<uint32_t>( ( *cursor++ ) ) << 16;
        temp += static_cast<uint32_t>( ( *cursor++ ) ) << 8;
        temp += static_cast<uint32_t>( ( *cursor++ ) );

        result.push_back( charset[( temp & 0x00FC0000 ) >> 18] );
        result.push_back( charset[( temp & 0x0003F000 ) >> 12] );
        result.push_back( charset[( temp & 0x00000FC0 ) >> 6] );
        result.push_back( charset[( temp & 0x0000003F )] );
    }

    switch ( bytes % 3 )
    {
        case 1:
            temp = static_cast<uint32_t>( ( *cursor++ ) ) << 16;
            result.push_back( charset[( temp & 0x00FC0000 ) >> 18] );
            result.push_back( charset[( temp & 0x0003F000 ) >> 12] );
            result.push_back( '=' );
            result.push_back( '=' );
            break;
        case 2:
            temp = static_cast<uint32_t>( ( *cursor++ ) ) << 16;
            temp += static_cast<uint32_t>( ( *cursor++ ) ) << 8;
            result.push_back( charset[( temp & 0x00FC0000 ) >> 18] );
            result.push_back( charset[( temp & 0x0003F000 ) >> 12] );
            result.push_back( charset[( temp & 0x00000FC0 ) >> 6] );
            result.push_back( '=' );
            break;
    }

    return result;
}

////////////////////////////////////////

std::string base64_decode( const void *data, size_t bytes )
{
    std::string result;

    const uint8_t *in  = static_cast<const uint8_t *>( data );
    const uint8_t *end = in + bytes;

    size_t iter = 0;
    size_t buf  = 0;

    while ( in < end )
    {
        unsigned char c = reverse[*in++];

        switch ( c )
        {
            case WHITESPACE:
                // skip whitespace
                continue;

            case INVALID: throw_runtime( "invalid base64 character" );

            case PADDING:
                // pad character, end of data
                in = end;
                continue;

            default:
                buf = buf << 6 | c;
                iter++;
                // If the buffer is full, split it into bytes
                if ( iter == 4 )
                {
                    result.push_back( static_cast<std::string::value_type>(
                        ( buf >> 16 ) & 0xff ) );
                    result.push_back( static_cast<std::string::value_type>(
                        ( buf >> 8 ) & 0xff ) );
                    result.push_back(
                        static_cast<std::string::value_type>( buf & 0xff ) );
                    buf  = 0;
                    iter = 0;
                }
                break;
        }
    }

    if ( iter == 3 )
    {
        result.push_back(
            static_cast<std::string::value_type>( ( buf >> 10 ) & 0xff ) );
        result.push_back(
            static_cast<std::string::value_type>( ( buf >> 2 ) & 0xff ) );
    }
    else if ( iter == 2 )
        result.push_back(
            static_cast<std::string::value_type>( ( buf >> 4 ) & 0xff ) );

    return result;
}

////////////////////////////////////////

} // namespace base
