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

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <map>
#include "const_string.h"
#include <ostream>

////////////////////////////////////////

namespace base
{

///
/// @brief Class spooky_hash provides an implementation of
///        Bob Jenkin's SpookyHashV2
///
/// while he provides code for spooky hash, we have chosen to reimplement here to add streaming operators        
///
class spooky_hash
{
public:
	typedef std::array<uint64_t, 2> value;

	spooky_hash( void );
	explicit spooky_hash( const value &seed );
	~spooky_hash( void );

	void reset( void );
	void reset( const value &seed );

	void add( const void *msg, size_t len );

	value final( void ) const;

	static value hash128( const void *msg, size_t len, const value &seed );
	static inline uint64_t hash64( const void *msg, size_t len, uint64_t seed );
	static inline uint32_t hash32( const void *msg, size_t len, uint32_t seed );

private:
	static const size_t stateSize = 12;
	static const size_t blockSize = stateSize * 8;
	static const size_t bufSize = blockSize * 2;

	mutable std::array<uint64_t, 2*stateSize> _data;
	std::array<uint64_t, stateSize> _state;

	size_t _length = 0;
	size_t _remainder = 0; // data in _data
};

////////////////////////////////////////

inline uint64_t
spooky_hash::hash64( const void *message, size_t length, uint64_t seed )
{
	value sd{{ seed, seed }};
	sd = hash128( message, length, sd );
	return sd[0];
}
		
inline uint32_t
spooky_hash::hash32( const void *message, size_t length, uint32_t seed )
{
	value sd{{ seed, seed }};
	sd = hash128( message, length, sd );
	return static_cast<uint32_t>( sd[0] );
}

////////////////////////////////////////

spooky_hash &operator <<( spooky_hash &h, bool v );
spooky_hash &operator <<( spooky_hash &h, unsigned char v );
spooky_hash &operator <<( spooky_hash &h, unsigned short v );
spooky_hash &operator <<( spooky_hash &h, unsigned int v );
spooky_hash &operator <<( spooky_hash &h, unsigned long v );
spooky_hash &operator <<( spooky_hash &h, unsigned long long v );
spooky_hash &operator <<( spooky_hash &h, signed char v );
spooky_hash &operator <<( spooky_hash &h, short v );
spooky_hash &operator <<( spooky_hash &h, int v );
spooky_hash &operator <<( spooky_hash &h, long v );
spooky_hash &operator <<( spooky_hash &h, long long v );
spooky_hash &operator <<( spooky_hash &h, float v );
spooky_hash &operator <<( spooky_hash &h, double v );
spooky_hash &operator <<( spooky_hash &h, const std::string &v );
spooky_hash &operator <<( spooky_hash &h, const cstring &s );
inline spooky_hash &operator <<( spooky_hash &h, const char *s )
{
	h << cstring( s );
	return h;
}

template <std::size_t N>
inline spooky_hash &operator <<( spooky_hash &h, const char (&s)[N] )
{
	h << cstring( s, N );
	return h;
}
spooky_hash &operator <<( spooky_hash &h, const spooky_hash &x );

template <typename V>
inline spooky_hash &operator <<( spooky_hash &h, const std::vector<V> &v )
{
	// can't add as a big block to handle case of array of strings
	for ( auto &e: v )
		h << e;
	return h;
}

template <typename V, size_t N>
inline spooky_hash &operator <<( spooky_hash &h, const std::array<V, N> &v )
{
	// can't add as a big block to handle case of array of strings
	for ( auto &e: v )
		h << e;
	return h;
}

template <typename K, typename V>
inline spooky_hash &operator <<( spooky_hash &h, const std::map<K, V> &v )
{
	for ( auto &e: v )
		h << e.first << e.second;
	return h;
}

template <typename A, typename B>
inline spooky_hash &operator <<( spooky_hash &h, const std::pair<A, B> &v )
{
	return h << v.first << v.second;
}

std::ostream &operator<<( std::ostream &os, const spooky_hash &h );
std::ostream &operator<<( std::ostream &os, const spooky_hash::value &v );


////////////////////////////////////////

inline bool operator==( const spooky_hash::value &a, const spooky_hash::value &b )
{
	return a[0] == b[0] && a[1] == b[1];
}
inline bool operator!=( const spooky_hash::value &a, const spooky_hash::value &b )
{
	return !( a == b );
}
inline bool operator<( const spooky_hash::value &a, const spooky_hash::value &b )
{
	return a[0] < b[0] || ( a[0] == b[0] && a[1] < b[1] );
}
	
} // namespace base



