//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>

////////////////////////////////////////

namespace color
{

namespace detail
{

template <int fbits, int tbits>
struct BitMapperHelp
{
};

template <>
struct BitMapperHelp<32, 32>
{
	static constexpr inline float conv( float a ) { return a; }
	static constexpr inline uint32_t conv( uint32_t a ) { return a; }
	// TODO: what should we do here for real?
	static constexpr inline uint32_t conv( float a ) { return static_cast<uint32_t>( a ); }
	static constexpr inline float conv( uint32_t a ) { return static_cast<float>( a ); }
};

template <>
struct BitMapperHelp<64, 64>
{
	static constexpr inline double conv( double a ) { return a; }
};

template <>
struct BitMapperHelp<64, 32>
{
	static constexpr inline float conv( double a ) { return float{ a }; }
};

/////////////////// 16 bit mappings ////////////////////////

template <>
struct BitMapperHelp<16, 16>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<16, 14>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(16383 << 2); }
};

template <>
struct BitMapperHelp<16, 12>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(4095 << 4); }
};

template <>
struct BitMapperHelp<16, 10>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<16, 8>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(255 << 8); }
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 14 bit mappings ////////////////////////

template <>
struct BitMapperHelp<14, 16>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<14, 14>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<14, 12>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(4095 << 4); }
};

template <>
struct BitMapperHelp<14, 10>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<14, 8>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(255 << 8); }
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 12 bit mappings ////////////////////////

template <>
struct BitMapperHelp<12, 16>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 14>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 12>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 10>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(1023 << 6); }
};

template <>
struct BitMapperHelp<12, 8>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(255 << 8); }
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 10 bit mappings ////////////////////////

template <>
struct BitMapperHelp<10, 16>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 14>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 12>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<10, 10>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a; }
};

template <>
struct BitMapperHelp<12, 8>
{
	static constexpr inline uint16_t conv( uint16_t a ) { return a & uint16_t(255 << 8); }
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a ); }
};

/////////////////// 8 bit mappings ////////////////////////

template <>
struct BitMapperHelp<8, 8>
{
	template <typename T, typename F>
	static constexpr inline typename std::enable_if<std::is_integer<T>::value && std::is_integer<F>::value, T>::type
	conv( F a ) { return static_cast<T>( a ); }
};

template <>
struct BitMapperHelp<8, 10>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 12>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 14>
{
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ) << 8; }
};

template <>
struct BitMapperHelp<8, 16>
{
	// TODO: add support for half
	template <typename T>
	static constexpr inline typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integer<T>::value, T>::type
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
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint8_t a ) { return static_cast<T>( a ); }

	template <typename T>
	static constexpr inline
	typename std::enable_if<std::is_integer<T>::value, T>::type
	conv( uint16_t a ) { return static_cast<T>( a >> 8 ); }
};

}

template <typename From, int frombits, typename To, int tobits>
inline To convert_bits( From v )
{
	return detail::BitMapperHelp<frombits, tobits>::conv<To>( v );
}

} // namespace color



