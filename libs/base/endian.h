//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "compiler_support.h"
#include <cstdint>
#if defined(_MSC_VER) && _MSC_VER >= 1400  /* Visual Studio */
# include <cstdlib>
# define bswap_16 _byteswap_ushort
# define bswap_32 _byteswap_ulong
# define bswap_64 _byteswap_uint64
#elif (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 8)
# define bswap_16(x) __builtin_bswap16(x)
# define bswap_32(x) __builtin_bswap32(x)
# define bswap_64(x) __builtin_bswap64(x)
#elif defined(__clang__)
# if (defined(__has_builtin) && __has_builtin(__builtin_bswap16))
#  define bswap_16(x) __builtin_bswap16(x)
#  define bswap_32(x) __builtin_bswap32(x)
#  define bswap_64(x) __builtin_bswap64(x)
# else
#  define bswap_16(x) __builtin_bswap16(x)
#  define bswap_32(x) __builtin_bswap32(x)
#  define bswap_64(x) __builtin_bswap64(x)
# endif
#elif defined(__linux__)
# include_next <endian.h>
# include <byteswap.h>
#else
# include <algorithm>
# include <utility>

template <typename T>
inline T bswap_16( T x )
{
	static_assert( sizeof(T) == 2, "invalid size object passed to bswap_16" );
	union
	{
		T n;
		uint8_t c[2];
	} bytes;
	bytes.n = x;
	std::swap( bytes.c[0], bytes.c[1] );
	return bytes.n;
}

template <typename T>
inline T bswap_32( T x )
{
	static_assert( sizeof(T) == 4, "invalid size object passed to bswap_32" );
	union
	{
		T n;
		uint8_t c[4];
	} bytes;
	bytes.n = c;
	std::swap( bytes.c[0], bytes.c[3] );
	std::swap( bytes.c[1], bytes.c[2] );
	return bytes.n;
}

template <typename T>
inline T bswap_64( T x )
{
	static_assert( sizeof(T) == 8, "invalid size object passed to bswap_64" );
	union
	{
		T n;
		uint8_t c[8];
	} bytes;
	bytes.n = c;
	std::swap( bytes.c[0], bytes.c[7] );
	std::swap( bytes.c[1], bytes.c[6] );
	std::swap( bytes.c[2], bytes.c[5] );
	std::swap( bytes.c[3], bytes.c[4] );
	return bytes.n;
}
#endif

namespace base
{

enum class endianness : uint8_t
{
	BIG,
	LITTLE,
#if defined(__LITTLE_ENDIAN__) || ( defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN) ) || ( defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN) ) || defined(_WIN32) || defined(WIN32)
	NATIVE = LITTLE,
#else // TODO: handle pdp endianness????
	NATIVE = BIG,
#endif
};

inline constexpr bool is_little_endian( void ) 
{
	return endianness::LITTLE == endianness::NATIVE;
}

inline constexpr bool is_big_endian( void ) 
{
	return endianness::BIG == endianness::NATIVE;
}

namespace priv
{

// unable to use constexpr for these since bswap_16, et al. end up
// with assembly, but these functions are unlikely to be called with
// constant values anyway
template <typename T, size_t>
struct bswap_helper
{
};

template <typename T>
struct bswap_helper<T,1ul>
{
	static inline T swap( T v ) { return v; }
	static inline T hosttobe( T v ) { return v; }
	static inline T hosttole( T v ) { return v; }
};

template <typename T>
struct bswap_helper<T,2ul>
{
	static inline T swap( T v )
	{
		return bswap_16( v );
	}
	static inline T hosttobe( T v )
	{
		if ( is_little_endian() )
			return swap( v );
		return v;
	}
	static inline T hosttole( T v )
	{
		if ( is_big_endian() )
			return swap( v );
		return v;
	}
};

template <typename T>
struct bswap_helper<T,4ul>
{
	static inline constexpr T swap( T v )
	{
		return bswap_32( v );
	}
	static inline T hosttobe( T v )
	{
		if ( is_little_endian() )
			return swap( v );
		return v;
	}
	static inline T hosttole( T v )
	{
		if ( is_big_endian() )
			return swap( v );
		return v;
	}
};

template <typename T>
struct bswap_helper<T,8ul>
{
	static inline T swap( T v )
	{
		return bswap_64( v );
	}
	static inline T hosttobe( T v )
	{
		if ( is_little_endian() )
			return swap( v );
		return v;
	}
	static inline T hosttole( T v )
	{
		if ( is_big_endian() )
			return swap( v );
		return v;
	}
};

}

template <typename T>
inline T byteswap( T v )
{
	return priv::bswap_helper<T,sizeof(T)>::swap( v );
}

////////////////////////////////////////

template <typename T>
inline T host2be( T c )
{
	return priv::bswap_helper<T,sizeof(T)>::hosttobe( c );
}

////////////////////////////////////////

template <typename T>
inline T host2le( T c )
{
	return priv::bswap_helper<T,sizeof(T)>::hosttole( c );
}

////////////////////////////////////////

}
