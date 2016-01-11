
#include <cstdint>
#include <string>
#include <cstring>
#include "sha256.h"
#include "contract.h"

namespace
{

////////////////////////////////////////

constexpr uint32_t BLOCK_SIZE = 64;

////////////////////////////////////////

inline uint32_t shfr( uint32_t x, uint32_t n )
{
	return ( x >> n );
}

////////////////////////////////////////

inline uint32_t rotr( uint32_t x, uint32_t n )
{
	return ( ( x >> n ) | ( x << ((sizeof(x) << 3) - n ) ) );
}

////////////////////////////////////////

inline uint32_t ch( uint32_t x,  uint32_t y,  uint32_t z )
{
	return ((x & y) ^ (~x & z));
}

////////////////////////////////////////

inline uint32_t maj( uint32_t x,  uint32_t y,  uint32_t z )
{
	return ((x & y) ^ (x & z) ^ (y & z));
}

////////////////////////////////////////

inline uint32_t f1( uint32_t x )
{
	return (rotr(x,  2) ^ rotr(x, 13) ^ rotr(x, 22));
}

////////////////////////////////////////

inline uint32_t f2( uint32_t x )
{
	return (rotr(x,  6) ^ rotr(x, 11) ^ rotr(x, 25));
}

////////////////////////////////////////

inline uint32_t f3( uint32_t x )
{
	return (rotr(x,  7) ^ rotr(x, 18) ^ shfr(x,  3));
}

////////////////////////////////////////

inline uint32_t f4( uint32_t x )
{
	return (rotr(x, 17) ^ rotr(x, 19) ^ shfr(x, 10));
}

////////////////////////////////////////

inline void unpack32( uint32_t x, uint8_t *str )
{
    str[3] = uint8_t( x       );
    str[2] = uint8_t( x >>  8 );
    str[1] = uint8_t( x >> 16 );
    str[0] = uint8_t( x >> 24 );
}

////////////////////////////////////////

inline void pack32( const uint8_t *str, uint32_t &x )
{
    x =   uint32_t( str[3]       )
        | uint32_t( str[2] <<  8 )
        | uint32_t( str[1] << 16 )
        | uint32_t( str[0] << 24 );
}

////////////////////////////////////////

const uint32_t K[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

////////////////////////////////////////

}

namespace base
{

////////////////////////////////////////

void sha256::update( const void *m, size_t len )
{
	precondition( !_finalized, "sha256 is already finalized" );

	const uint8_t *msg = reinterpret_cast<const uint8_t *>( m );
	size_t block_nb;
	size_t new_len, rem_len, tmp_len;
	const uint8_t *shifted_msg;
	tmp_len = BLOCK_SIZE - _len;
	rem_len = len < tmp_len ? len : tmp_len;
	memcpy( &_block[_len], msg, rem_len );
	if ( _len + len < BLOCK_SIZE )
	{
		_len += len;
		return;
	}
	new_len = len - rem_len;
	block_nb = new_len / BLOCK_SIZE;
	shifted_msg = msg + rem_len;
	transform( _block, 1 );
	transform( shifted_msg, block_nb );
	rem_len = new_len % BLOCK_SIZE;
	memcpy( _block, &shifted_msg[block_nb << 6], rem_len );
	_len = rem_len;
	_tot_len += ( block_nb + 1 ) << 6;
}

////////////////////////////////////////

sha256::hash sha256::get_hash( void )
{
	if ( !_finalized )
		finalize();
	hash result;
	for ( size_t i = 0 ; i < 8; i++ )
		unpack32( _hash[i], result.data() + i*4 );
	return result;
}

////////////////////////////////////////

std::string sha256::hash_string( void )
{
	if ( !_finalized )
		finalize();
	std::string result;
	uint8_t buf[4];
	char tmp[3];
	for ( size_t i = 0 ; i < 8; i++ )
	{
		unpack32( _hash[i], buf );

		for ( size_t j = 0; j < 4; ++j )
		{
			sprintf( tmp, "%02x", uint32_t(buf[j]) );
			result.append( tmp, tmp+2 );
		}
	}

	return result;
}

////////////////////////////////////////

void sha256::transform( const void *m, size_t block_nb )
{
	const uint8_t *msg = reinterpret_cast<const uint8_t*>( m );
	uint32_t w[64];
	uint32_t wv[8];
	uint32_t t1, t2;
	const uint8_t *sub_block;

	for ( size_t i = 0; i < block_nb; i++ )
	{
		sub_block = msg + ( i << 6 );
		for ( size_t j = 0; j < 16; j++ )
			pack32( &sub_block[j << 2], w[j] );
		for ( size_t j = 16; j < 64; j++ )
			w[j] =  f4( w[j -  2] ) + w[j -  7] + f3( w[j - 15] ) + w[j - 16];
		for ( size_t j = 0; j < 8; j++ )
			wv[j] = _hash[j];
		for ( size_t j = 0; j < 64; j++ )
		{
			t1 = wv[7] + f2( wv[4] ) + ch( wv[4], wv[5], wv[6] )
			     + K[j] + w[j];
			t2 = f1( wv[0] ) + maj( wv[0], wv[1], wv[2] );
			wv[7] = wv[6];
			wv[6] = wv[5];
			wv[5] = wv[4];
			wv[4] = wv[3] + t1;
			wv[3] = wv[2];
			wv[2] = wv[1];
			wv[1] = wv[0];
			wv[0] = t1 + t2;
		}
		for ( size_t j = 0; j < 8; j++ )
			_hash[j] += wv[j];
	}
}

////////////////////////////////////////

void sha256::finalize( void )
{
	size_t block_nb = ( 1 + ( ( BLOCK_SIZE - 9 ) < ( _len % BLOCK_SIZE ) ) );
	size_t len_b = ( _tot_len + _len ) << 3;
	size_t pm_len = block_nb << 6;
	memset( _block + _len, 0, pm_len - _len );
	_block[_len] = 0x80;
	unpack32( static_cast<uint32_t>( len_b ), _block + pm_len - 4 );
	transform( _block, block_nb );
	_finalized = true;
}

////////////////////////////////////////

}
