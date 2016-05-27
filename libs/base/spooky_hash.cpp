//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "spooky_hash.h"
#include <iomanip>

////////////////////////////////////////

namespace
{
inline uint64_t rot64( uint64_t x, int k )
{
	return ( x << k ) | ( x >> ( 64 - k ) );
}

inline void mix( const uint64_t *data,
				 std::array<uint64_t, 12> &s )
{
	s[0] += data[0]; s[2] ^= s[10]; s[11] ^= s[0]; s[0] = rot64(s[0],11); s[11] += s[1];
	s[1] += data[1]; s[3] ^= s[11]; s[0] ^= s[1]; s[1] = rot64(s[1],32); s[0] += s[2];
	s[2] += data[2]; s[4] ^= s[0]; s[1] ^= s[2]; s[2] = rot64(s[2],43); s[1] += s[3];
	s[3] += data[3]; s[5] ^= s[1]; s[2] ^= s[3]; s[3] = rot64(s[3],31); s[2] += s[4];
	s[4] += data[4]; s[6] ^= s[2]; s[3] ^= s[4]; s[4] = rot64(s[4],17); s[3] += s[5];
	s[5] += data[5]; s[7] ^= s[3]; s[4] ^= s[5]; s[5] = rot64(s[5],28); s[4] += s[6];
	s[6] += data[6]; s[8] ^= s[4]; s[5] ^= s[6]; s[6] = rot64(s[6],39); s[5] += s[7];
	s[7] += data[7]; s[9] ^= s[5]; s[6] ^= s[7]; s[7] = rot64(s[7],57); s[6] += s[8];
	s[8] += data[8]; s[10] ^= s[6]; s[7] ^= s[8]; s[8] = rot64(s[8],55); s[7] += s[9];
	s[9] += data[9]; s[11] ^= s[7]; s[8] ^= s[9]; s[9] = rot64(s[9],54); s[8] += s[10];
	s[10] += data[10]; s[0] ^= s[8]; s[9] ^= s[10]; s[10] = rot64(s[10],22); s[9] += s[11];
	s[11] += data[11]; s[1] ^= s[9]; s[10] ^= s[11]; s[11] = rot64(s[11],46); s[10] += s[0];
}

inline void endPartial( std::array<uint64_t, 12> &h )
{
	h[11] += h[1]; h[2] ^= h[11]; h[1] = rot64( h[1], 44 );
	h[0] += h[2]; h[3] ^= h[0]; h[2] = rot64( h[2], 15 );
	h[1] += h[3]; h[4] ^= h[1]; h[3] = rot64( h[3], 34 );
	h[2] += h[4]; h[5] ^= h[2]; h[4] = rot64( h[4], 21 );
	h[3] += h[5]; h[6] ^= h[3]; h[5] = rot64( h[5], 38 );
	h[4] += h[6]; h[7] ^= h[4]; h[6] = rot64( h[6], 33 );
	h[5] += h[7]; h[8] ^= h[5]; h[7] = rot64( h[7], 10 );
	h[6] += h[8]; h[9] ^= h[6]; h[8] = rot64( h[8], 13 );
	h[7] += h[9]; h[10] ^= h[7]; h[9] = rot64( h[9], 38 );
	h[8] += h[10]; h[11] ^= h[8]; h[10] = rot64( h[10], 53 );
	h[9] += h[11]; h[0] ^= h[9]; h[11] = rot64( h[11], 42 );
	h[10] += h[0]; h[1] ^= h[10]; h[0] = rot64( h[0], 54 );
}

inline void endMix( const uint64_t *data, std::array<uint64_t, 12> &h )
{
	for ( size_t i = 0; i != 12; ++i )
		h[i] += data[i];
	endPartial( h );
	endPartial( h );
	endPartial( h );
}

inline void shortMix( uint64_t &h0, uint64_t &h1, uint64_t &h2, uint64_t &h3 )
{
	h2 = rot64( h2, 50 ); h2 += h3; h0 ^= h2;
	h3 = rot64( h3, 52 ); h3 += h0; h1 ^= h3;
	h0 = rot64( h0, 30 ); h0 += h1; h2 ^= h0;
	h1 = rot64( h1, 41 ); h1 += h2; h3 ^= h1;

	h2 = rot64( h2, 54 ); h2 += h3; h0 ^= h2;
	h3 = rot64( h3, 48 ); h3 += h0; h1 ^= h3;
	h0 = rot64( h0, 38 ); h0 += h1; h2 ^= h0;
	h1 = rot64( h1, 37 ); h1 += h2; h3 ^= h1;

	h2 = rot64( h2, 62 ); h2 += h3; h0 ^= h2;
	h3 = rot64( h3, 34 ); h3 += h0; h1 ^= h3;
	h0 = rot64( h0, 5 ); h0 += h1; h2 ^= h0;
	h1 = rot64( h1, 36 ); h1 += h2; h3 ^= h1;
}

inline void shortEnd( uint64_t &h0, uint64_t &h1, uint64_t &h2, uint64_t &h3 )
{
	h3 ^= h2; h2 = rot64( h2, 15 ); h3 += h2;
	h0 ^= h3; h3 = rot64( h3, 52 ); h0 += h3;
	h1 ^= h0; h0 = rot64( h0, 26 ); h1 += h0;
	h2 ^= h1; h1 = rot64( h1, 51 ); h2 += h1;

	h3 ^= h2; h2 = rot64( h2, 28 ); h3 += h2;
	h0 ^= h3; h3 = rot64( h3, 9 ); h0 += h3;
	h1 ^= h0; h0 = rot64( h0, 47 ); h1 += h0;
	h2 ^= h1; h1 = rot64( h1, 54 ); h2 += h1;

	h3 ^= h2; h2 = rot64( h2, 32 ); h3 += h2;
	h0 ^= h3; h3 = rot64( h3, 25 ); h0 += h3;
	h1 ^= h0; h0 = rot64( h0, 63 ); h1 += h0;
	// no update for h2
}

static const uint64_t sc_const = 0xdeadbeefdeadbeefLL;

static void
short_msg( const void *message, size_t length, base::spooky_hash::value &hash )
{
	size_t remainder = length % 32;
	uint64_t a = hash[0];
	uint64_t b = hash[1];
	uint64_t c = sc_const;
	uint64_t d = sc_const;

	const uint64_t *p64 = reinterpret_cast<const uint64_t *>( message );

	if ( length > 15 )
	{
		const uint64_t *end = p64 + (length / 32) * 4;

		while ( p64 < end )
		{
			c += p64[0];
			d += p64[1];
			shortMix( a, b, c, d );
			a += p64[2];
			b += p64[3];
			p64 += 4;
		}

		if ( remainder >= 16 )
		{
			c += p64[0];
			d += p64[1];
			shortMix( a, b, c, d );
			p64 += 2;
			remainder -= 16;
		}
	}

	d += static_cast<uint64_t>( length ) << 56;
	const uint8_t *p8 = reinterpret_cast<const uint8_t *>( p64 );
	const uint32_t *p32 = reinterpret_cast<const uint32_t *>( p64 );
	switch ( remainder )
	{
		case 15: d += static_cast<uint64_t>( p8[14] ) << 48; [[fallthrough]]
		case 14: d += static_cast<uint64_t>( p8[13] ) << 40; [[fallthrough]]
		case 13: d += static_cast<uint64_t>( p8[12] ) << 32; [[fallthrough]]
		case 12:
			d += static_cast<uint64_t>( p32[2] );
			c += p64[0];
			break;
		case 11: d += static_cast<uint64_t>( p8[10] ) << 16; [[fallthrough]]
		case 10: d += static_cast<uint64_t>( p8[9] ) << 8; [[fallthrough]]
		case 9: d += static_cast<uint64_t>( p8[8] ); [[fallthrough]]
		case 8:
			c += p64[0];
			break;

		case 7: c += static_cast<uint64_t>( p8[6] ) << 48; [[fallthrough]]
		case 6: c += static_cast<uint64_t>( p8[5] ) << 40; [[fallthrough]]
		case 5: c += static_cast<uint64_t>( p8[4] ) << 32; [[fallthrough]]
		case 4:
			c += p32[0];
			break;

		case 3: c += static_cast<uint64_t>( p8[2] ) << 16; [[fallthrough]]
		case 2: c += static_cast<uint64_t>( p8[1] ) << 8; [[fallthrough]]
		case 1:
			c += static_cast<uint64_t>( p8[0] );
			break;
		case 0:
			c += sc_const;
			d += sc_const;
	}
	shortEnd( a, b, c, d );
	hash[0] = a;
	hash[1] = b;
}

} // empty namespace

namespace base
{

////////////////////////////////////////

spooky_hash::spooky_hash( void )
{
	_state[0] = 0;
	_state[1] = 0;
}

////////////////////////////////////////

spooky_hash::spooky_hash( const value &seed )
{
	_state[0] = seed[0];
	_state[1] = seed[1];
}

////////////////////////////////////////

spooky_hash::~spooky_hash( void )
{
}

////////////////////////////////////////

void
spooky_hash::reset( void )
{
	_length = 0;
	_remainder = 0;
	_state[0] = 0;
	_state[1] = 0;
}

////////////////////////////////////////

void
spooky_hash::reset( const value &seed )
{
	_length = 0;
	_remainder = 0;
	_state[0] = seed[0];
	_state[1] = seed[1];
}

////////////////////////////////////////

void
spooky_hash::add( const void *msg, size_t len )
{
	size_t newLen = len + _remainder;

	// if we don't yet have enough data to mix, stash it
	if ( newLen < bufSize )
	{
		uint8_t *p8 = reinterpret_cast<uint8_t *>( _data.data() );
		memcpy( p8 + _remainder, msg, len );
		_length += len;
		_remainder = newLen;
		return;
	}

	std::array<uint64_t, stateSize> h;

	if ( _length < bufSize )
	{
		h[0] = h[3] = h[6] = h[9] = _state[0];
		h[1] = h[4] = h[7] = h[10] = _state[1];
		h[2] = h[5] = h[8] = h[11] = sc_const;
	}
	else
	{
		h = _state;
	}
	_length += len;

	const uint64_t *p64 = reinterpret_cast<const uint64_t *>( msg );
	if ( _remainder )
	{
		uint8_t *p8 = reinterpret_cast<uint8_t *>( _data.data() );
		size_t prefix = bufSize - _remainder;
		memcpy( p8 + _remainder, msg, prefix );
		mix( _data.data(), h );
		mix( _data.data() + stateSize, h );
		p64 = reinterpret_cast<const uint64_t *>( ( reinterpret_cast<const uint8_t *>( msg ) + prefix ) );
		len -= prefix;
	}

	const uint64_t *end = p64 + ( len / blockSize ) * stateSize;

	_remainder = ( len - static_cast<size_t>( reinterpret_cast<const uint8_t *>( end ) - reinterpret_cast<const uint8_t *>( p64 ) ) );
	while ( p64 < end )
	{
		mix( p64, h );
		p64 += stateSize;
	}
	memcpy( _data.data(), end, _remainder );
	_state = h;
}

////////////////////////////////////////

spooky_hash::value
spooky_hash::final( void ) const
{
	value ret;
	if ( _length < bufSize )
	{
		ret[0] = _state[0];
		ret[1] = _state[1];
		short_msg( _data.data(), _length, ret );
		return ret;
	}

	std::array<uint64_t, stateSize> h = _state;

	uint64_t *data = _data.data();
	size_t remainder = _remainder;
	if ( _remainder >= blockSize )
	{
		mix( data, h );
		data += stateSize;
		remainder -= blockSize;
	}

	uint8_t *p8 = reinterpret_cast<uint8_t *>( data );
	memset( p8 + remainder, 0, (blockSize - remainder) );
	p8[blockSize - 1] = static_cast<uint8_t>( remainder );
	endMix( data, h );
	ret[0] = h[0];
	ret[1] = h[1];
	return ret;
}

////////////////////////////////////////

spooky_hash::value
spooky_hash::hash128( const void *message, size_t length, const value &seed )
{
	value ret = seed;
	if ( length < bufSize )
	{
		short_msg( message, length, ret );
		return ret;
	}

	std::array<uint64_t, stateSize> h;

	h[0] = h[3] = h[6] = h[9] = seed[0];
	h[1] = h[4] = h[7] = h[10] = seed[1];
	h[2] = h[5] = h[8] = h[11] = sc_const;

	const uint64_t *p64 = reinterpret_cast<const uint64_t *>( message );
	const uint64_t *end = p64 + ( length / blockSize ) * stateSize;
	while ( p64 < end )
	{
		mix( p64, h );
		p64 += stateSize;
	}

	const uint8_t *cend = reinterpret_cast<const uint8_t *>( end );
	const uint8_t *p8 = reinterpret_cast<const uint8_t *>( message );
	size_t remainder = ( length - static_cast<size_t>( cend - p8 ) );
	// use value initialization to get 0 in to all values
	std::array<uint64_t, stateSize> buf{};

	uint64_t *pBuf = buf.data();
	uint8_t *p8buf = reinterpret_cast<uint8_t *>( pBuf );
	memcpy( pBuf, end, remainder );
	p8buf[blockSize-1] = static_cast<uint8_t>( remainder );
	endMix( pBuf, h );
	ret[0] = h[0];
	ret[1] = h[1];
	return ret;
}

////////////////////////////////////////

spooky_hash &operator <<( spooky_hash &h, bool v )
{
	uint8_t x = v ? 1 : 0;
	h.add( &x, sizeof(uint8_t) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, unsigned char v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, unsigned short v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, unsigned int v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, unsigned long v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, unsigned long long v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, signed char v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, short v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, int v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, long v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, long long v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, float v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, double v )
{
	h.add( &v, sizeof(v) );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, const std::string &v )
{
	h.add( v.c_str(), v.size() );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, const cstring &s )
{
	h.add( s.c_str(), s.size() );
	return h;
}

spooky_hash &operator <<( spooky_hash &h, const spooky_hash &x )
{
	h << x.final();
	return h;
}

////////////////////////////////////////

std::ostream &
operator<<( std::ostream &os, const spooky_hash &h )
{
	os << h.final();
	return os;
}

////////////////////////////////////////

std::ostream &
operator<<( std::ostream &os, const spooky_hash::value &v )
{
	os << std::hex << std::setw(16) << std::setfill( '0' ) << v[0] << std::setw(16) << std::setfill( '0' ) << v[1] << std::dec;
	return os;
}

////////////////////////////////////////

} // base



