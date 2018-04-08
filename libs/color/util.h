//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <type_traits>

////////////////////////////////////////

namespace color
{

namespace detail
{

template <int fbits, int tbits>
struct BitMapperHelp
{
};

/////////////////// 32 bit mappings ////////////////////////

template <>
struct BitMapperHelp<32, 32>
{
	// TODO: what should we do here for real?
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint32_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( float a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<32, 16>
{
	template <typename T>
	static constexpr inline uint16_t
	conv( typename std::enable_if<std::is_integral<T>::value, T>::type a )
	{ return static_cast<uint16_t>( a ); }

	template <typename T>
	static constexpr inline T conv( float a )
	{ return static_cast<T>( ( a + 0.5f ) * 65535.f ); }
};

template <>
struct BitMapperHelp<32, 14>
{
	template <typename T>
	static constexpr inline T
	conv( typename std::enable_if<std::is_integral<T>::value, T>::type a )
	{ return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline T conv( float a )
	{
		return static_cast<T>(
			static_cast<uint16_t>( ( a + 0.5f ) * 16383.f ) << 2 );
	}
};

template <>
struct BitMapperHelp<32, 12>
{
	template <typename T>
	static constexpr inline uint16_t
	conv( typename std::enable_if<std::is_integral<T>::value, T>::type a )
	{ return static_cast<uint16_t>( a ); }

	template <typename T>
	static constexpr inline T conv( float a )
	{
		return static_cast<T>(
			static_cast<uint16_t>( ( a + 0.5f ) * 4095.f ) << 4 );
	}
};

template <>
struct BitMapperHelp<32, 10>
{
	template <typename T>
	static constexpr inline uint16_t
	conv( typename std::enable_if<std::is_integral<T>::value, T>::type a )
	{ return static_cast<uint16_t>( a ); }

	template <typename T>
	static constexpr inline T conv( float a )
	{
		return static_cast<T>(
			static_cast<uint16_t>( ( a + 0.5f ) * 1023.f ) << 6 );
	}
};

template <>
struct BitMapperHelp<32, 8>
{
	template <typename T>
	static constexpr inline uint8_t
	conv( typename std::enable_if<std::is_integral<T>::value, T>::type a )
	{ return static_cast<uint8_t>( a ); }

	template <typename T>
	static constexpr inline uint8_t conv( float a )
	{
		return static_cast<uint8_t>( ( a + 0.5f ) * 255.f );
	}
};

/////////////////// 64 bit mappings ////////////////////////

template <>
struct BitMapperHelp<64, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint64_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( double a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<64, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( double a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( double a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<64, 16>
{
	template <typename T>
	static constexpr inline T conv( double a ) { return BitMapperHelp<32,16>::conv<T>( float( a ) ); }
};

template <>
struct BitMapperHelp<64, 14>
{
	template <typename T>
	static constexpr inline T conv( double a ) { return BitMapperHelp<32,14>::conv<T>( float( a ) ); }
};

template <>
struct BitMapperHelp<64, 12>
{
	template <typename T>
	static constexpr inline T conv( double a ) { return BitMapperHelp<32,12>::conv<T>( float( a ) ); }
};

template <>
struct BitMapperHelp<64, 10>
{
	template <typename T>
	static constexpr inline T conv( double a ) { return BitMapperHelp<32,10>::conv<T>( float( a ) ); }
};

template <>
struct BitMapperHelp<64, 8>
{
	template <typename T>
	static constexpr inline T conv( double a ) { return BitMapperHelp<32,8>::conv<T>( float( a ) ); }
};

/////////////////// 16 bit mappings ////////////////////////

template <>
struct BitMapperHelp<16, 16>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<16, 14>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(16383 << 2); }
};

template <>
struct BitMapperHelp<16, 12>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(4095 << 4); }
};

template <>
struct BitMapperHelp<16, 10>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<16, 8>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return uint8_t( a >> 8 ); }
};

template <>
struct BitMapperHelp<16, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ) / T(65535); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<16, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ) / T(65535); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 14 bit mappings ////////////////////////

template <>
struct BitMapperHelp<14, 16>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<14, 14>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<14, 12>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(4095 << 4); }
};

template <>
struct BitMapperHelp<14, 10>
{
	template <typename T>
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<14, 8>
{
	template <typename T>
	static constexpr inline uint8_t conv( uint16_t a ) { return static_cast<uint8_t>( a >> 8 ); }
};

template <>
struct BitMapperHelp<14, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 2 ) / T(16383); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<14, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 2 ) / T(16383); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 12 bit mappings ////////////////////////

template <>
struct BitMapperHelp<12, 16>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 14>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 12>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 10>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<12, 8>
{
	template <typename T>
	static constexpr inline uint8_t conv( uint16_t a ) { return uint8_t( a >> 8 ); }
};

template <>
struct BitMapperHelp<12, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 4 ) / T(4095); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<12, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 4 ) / T(4095); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 10 bit mappings ////////////////////////

template <>
struct BitMapperHelp<10, 16>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 14>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 12>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 10>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 8>
{
	template <typename T>
	static constexpr inline T conv( uint16_t a ) { return static_cast<uint8_t>( a >> 8 ); }
};

template <>
struct BitMapperHelp<10, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 6 ) / T(1023); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<10, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 6 ) / T(1023); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 8 bit mappings ////////////////////////

template <>
struct BitMapperHelp<8, 8>
{
	template <typename T, typename F>
	static constexpr inline typename std::enable_if<std::is_integral<T>::value && std::is_integral<F>::value, T>::type
	conv( F a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<8, 10>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 12>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 14>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 16>
{
	// TODO: add support for half
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 32>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) / T(255); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( ( a >> 8 ) ) / T(255); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 8 ); }
};

template <>
struct BitMapperHelp<8, 64>
{
	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) / T(255); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( ( a >> 8 ) ) / T(255); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integral<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 8 ); }
};

}

template <typename From, int frombits, typename To, int tobits>
inline To convert_bits( From v )
{
	return detail::BitMapperHelp<frombits, tobits>::template conv<To>( v );
}

} // namespace color



