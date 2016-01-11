/*
	sha1.cpp - source code of

	============
	SHA-1 in C++
	============

	100% Public Domain.

	Original C Code
		-- Steve Reid <steve@edmweb.com>
	Small changes to fit into bglibs
		-- Bruce Guenter <bruce@untroubled.org>
	Translation to simpler C++ Code
		-- Volker Grabsch <vog@notjusthosting.com>
*/

#include "sha160.h"
#include "format.h"
#include "contract.h"
#include "endian.h"
#include <string.h>

namespace
{

constexpr uint32_t BLOCK_SIZE = 64;  // bytes per SHA1 block
constexpr uint32_t BLOCK_INTS = 16;  // number of 32bit integers per SHA1 block

}

namespace base
{

////////////////////////////////////////

sha160::hash sha160::get_hash( void )
{
	if ( !_finalized )
		finalize();

	hash result;
	for ( size_t i = 0; i < 5; ++i )
	{
		uint32_t v = base::host2be( _digest[i] );
		memcpy( result.data() + i*4, &v, sizeof( v ) );
	}
	return result;
}

////////////////////////////////////////

std::string sha160::hash_string( void )
{
	if ( !_finalized )
		finalize();

	std::string result;
	for ( size_t i = 0; i < 5; ++i )
		result += base::format( "{0,b16,w8,f0}", _digest[i] );

	return result;
}

////////////////////////////////////////

void sha160::update( const void *message, size_t bytes )
{
	precondition( !_finalized, "sha160 is already finalized" );
	const uint8_t *msg = static_cast<const uint8_t *>( message );

	if ( _len > 0 )
	{
		size_t n = std::min( bytes, 64 - _len );
		memcpy( reinterpret_cast<uint8_t*>( _block ) + _len, msg, n );
		_len += n;
		msg += n;
		bytes -= n;
	}

	if ( _len == 64 )
	{
		transform();
		_len = 0;
	}

	while ( bytes > 64 )
	{
		memcpy( _block, msg, 64 );
		transform();
		msg += 64;
		bytes -= 64;
	}

	if ( bytes > 0 )
	{
		_len = bytes;
		memcpy( _block, msg, _len );
	}
}

////////////////////////////////////////

// Add padding and return the message digest.
void sha160::finalize( void )
{
	// Total number of hashed bits
	uint64_t total_bits = ( _transforms * BLOCK_SIZE + _len ) * 8;

	// Padding
	const uint8_t tmp = 0x80;
	memcpy( reinterpret_cast<uint8_t*>( _block ) + _len, &tmp, 1 );
	++_len;
	memset( reinterpret_cast<uint8_t*>( _block ) + _len, 0, 64 - _len );

	if ( _len > BLOCK_SIZE - 8 )
	{
		transform();
		for ( size_t i = 0; i < BLOCK_INTS - 2; i++ )
			_block[i] = 0;
	}

	// Append total_bits, split this uint64 into two uint32
	_block[BLOCK_INTS - 1] = base::host2be( uint32_t( total_bits & 0xffffffff ) );
	_block[BLOCK_INTS - 2] = base::host2be( uint32_t( total_bits >> 32 ) );
	transform();
	_finalized = true;
}

////////////////////////////////////////

namespace
{

// Help functions
inline uint32_t SHA1_ROL( uint32_t value, uint32_t bits )
{
	return (((value) << (bits)) | (((value) & 0xffffffff) >> (32 - (bits))));
}

inline uint32_t SHA1_BLK( uint32_t *block, size_t i )
{
	return ( block[i&15] = SHA1_ROL( block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i&15], 1 ) );
}

// R0, R1, R2, R3, R4 are the different operations used in SHA1
inline void SHA1_R0( uint32_t *block, uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, size_t i )
{
	z += ( ( w & ( x ^ y ) ) ^ y ) + block[i] + 0x5a827999 + SHA1_ROL( v, 5 );
	w = SHA1_ROL( w, 30 );
}

inline void SHA1_R1( uint32_t *block, uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, size_t i )
{
	z += ((w&(x^y))^y) + SHA1_BLK( block, i ) + 0x5a827999 + SHA1_ROL( v, 5 );
	w = SHA1_ROL( w, 30 );
}

inline void SHA1_R2( uint32_t *block, uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, size_t i )
{
	z += (w^x^y) + SHA1_BLK( block, i ) + 0x6ed9eba1 + SHA1_ROL( v, 5 );
	w = SHA1_ROL( w, 30 );
}

inline void SHA1_R3( uint32_t *block, uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, size_t i )
{
	z += (((w|x)&y)|(w&x)) + SHA1_BLK( block, i ) + 0x8f1bbcdc + SHA1_ROL( v, 5 );
	w = SHA1_ROL( w, 30 );
}

inline void SHA1_R4( uint32_t *block, uint32_t v, uint32_t &w, uint32_t x, uint32_t y, uint32_t &z, size_t i )
{
	z += (w^x^y) + SHA1_BLK( block, i ) + 0xca62c1d6 + SHA1_ROL( v, 5 );
	w = SHA1_ROL( w, 30 );
}

}

// Hash a single 512-bit block. This is the core of the algorithm.
void sha160::transform( void )
{
	for ( size_t i = 0; i < BLOCK_INTS; ++i )
		_block[i] = host2be( _block[i] );

	// Copy digest[] to working vars
	uint32_t a = _digest[0];
	uint32_t b = _digest[1];
	uint32_t c = _digest[2];
	uint32_t d = _digest[3];
	uint32_t e = _digest[4];

	// 4 rounds of 20 operations each. Loop unrolled.
	SHA1_R0( _block, a, b, c, d, e, 0 );
	SHA1_R0( _block, e, a, b, c, d, 1 );
	SHA1_R0( _block, d, e, a, b, c, 2 );
	SHA1_R0( _block, c, d, e, a, b, 3 );
	SHA1_R0( _block, b, c, d, e, a, 4 );
	SHA1_R0( _block, a, b, c, d, e, 5 );
	SHA1_R0( _block, e, a, b, c, d, 6 );
	SHA1_R0( _block, d, e, a, b, c, 7 );
	SHA1_R0( _block, c, d, e, a, b, 8 );
	SHA1_R0( _block, b, c, d, e, a, 9 );
	SHA1_R0( _block, a, b, c, d, e, 10 );
	SHA1_R0( _block, e, a, b, c, d, 11 );
	SHA1_R0( _block, d, e, a, b, c, 12 );
	SHA1_R0( _block, c, d, e, a, b, 13 );
	SHA1_R0( _block, b, c, d, e, a, 14 );
	SHA1_R0( _block, a, b, c, d, e, 15 );
	SHA1_R1( _block, e, a, b, c, d, 16 );
	SHA1_R1( _block, d, e, a, b, c, 17 );
	SHA1_R1( _block, c, d, e, a, b, 18 );
	SHA1_R1( _block, b, c, d, e, a, 19 );
	SHA1_R2( _block, a, b, c, d, e, 20 );
	SHA1_R2( _block, e, a, b, c, d, 21 );
	SHA1_R2( _block, d, e, a, b, c, 22 );
	SHA1_R2( _block, c, d, e, a, b, 23 );
	SHA1_R2( _block, b, c, d, e, a, 24 );
	SHA1_R2( _block, a, b, c, d, e, 25 );
	SHA1_R2( _block, e, a, b, c, d, 26 );
	SHA1_R2( _block, d, e, a, b, c, 27 );
	SHA1_R2( _block, c, d, e, a, b, 28 );
	SHA1_R2( _block, b, c, d, e, a, 29 );
	SHA1_R2( _block, a, b, c, d, e, 30 );
	SHA1_R2( _block, e, a, b, c, d, 31 );
	SHA1_R2( _block, d, e, a, b, c, 32 );
	SHA1_R2( _block, c, d, e, a, b, 33 );
	SHA1_R2( _block, b, c, d, e, a, 34 );
	SHA1_R2( _block, a, b, c, d, e, 35 );
	SHA1_R2( _block, e, a, b, c, d, 36 );
	SHA1_R2( _block, d, e, a, b, c, 37 );
	SHA1_R2( _block, c, d, e, a, b, 38 );
	SHA1_R2( _block, b, c, d, e, a, 39 );
	SHA1_R3( _block, a, b, c, d, e, 40 );
	SHA1_R3( _block, e, a, b, c, d, 41 );
	SHA1_R3( _block, d, e, a, b, c, 42 );
	SHA1_R3( _block, c, d, e, a, b, 43 );
	SHA1_R3( _block, b, c, d, e, a, 44 );
	SHA1_R3( _block, a, b, c, d, e, 45 );
	SHA1_R3( _block, e, a, b, c, d, 46 );
	SHA1_R3( _block, d, e, a, b, c, 47 );
	SHA1_R3( _block, c, d, e, a, b, 48 );
	SHA1_R3( _block, b, c, d, e, a, 49 );
	SHA1_R3( _block, a, b, c, d, e, 50 );
	SHA1_R3( _block, e, a, b, c, d, 51 );
	SHA1_R3( _block, d, e, a, b, c, 52 );
	SHA1_R3( _block, c, d, e, a, b, 53 );
	SHA1_R3( _block, b, c, d, e, a, 54 );
	SHA1_R3( _block, a, b, c, d, e, 55 );
	SHA1_R3( _block, e, a, b, c, d, 56 );
	SHA1_R3( _block, d, e, a, b, c, 57 );
	SHA1_R3( _block, c, d, e, a, b, 58 );
	SHA1_R3( _block, b, c, d, e, a, 59 );
	SHA1_R4( _block, a, b, c, d, e, 60 );
	SHA1_R4( _block, e, a, b, c, d, 61 );
	SHA1_R4( _block, d, e, a, b, c, 62 );
	SHA1_R4( _block, c, d, e, a, b, 63 );
	SHA1_R4( _block, b, c, d, e, a, 64 );
	SHA1_R4( _block, a, b, c, d, e, 65 );
	SHA1_R4( _block, e, a, b, c, d, 66 );
	SHA1_R4( _block, d, e, a, b, c, 67 );
	SHA1_R4( _block, c, d, e, a, b, 68 );
	SHA1_R4( _block, b, c, d, e, a, 69 );
	SHA1_R4( _block, a, b, c, d, e, 70 );
	SHA1_R4( _block, e, a, b, c, d, 71 );
	SHA1_R4( _block, d, e, a, b, c, 72 );
	SHA1_R4( _block, c, d, e, a, b, 73 );
	SHA1_R4( _block, b, c, d, e, a, 74 );
	SHA1_R4( _block, a, b, c, d, e, 75 );
	SHA1_R4( _block, e, a, b, c, d, 76 );
	SHA1_R4( _block, d, e, a, b, c, 77 );
	SHA1_R4( _block, c, d, e, a, b, 78 );
	SHA1_R4( _block, b, c, d, e, a, 79 );

	// Add the working vars back into digest[]
	_digest[0] += a;
	_digest[1] += b;
	_digest[2] += c;
	_digest[3] += d;
	_digest[4] += e;

	// Count the number of transformations
	_transforms++;
}

////////////////////////////////////////

}

