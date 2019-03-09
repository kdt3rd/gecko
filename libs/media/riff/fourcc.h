// Copyright (c) 2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <iostream>

namespace media
{

////////////////////////////////////////

/// @brief Four character code used in both RIFF based files and in
/// quicktime containers.
///
class fourcc
{
public:
	/// @brief Construct invalid FourCC (0)
	constexpr fourcc( void )
	{
	}

	/// @brief Construct FourCC from literal string.
	template<size_t N>
	fourcc( const char (&c)[N] )
		: _char{ c[0], c[1], c[2], c[3] }
	{
		static_assert( N == 5, "fourcc must be 4 characters" );
	}

	/// @brief Read FourCC from stream.
	fourcc( std::istream &in );


	/// @brief Set FourCC.
	template<size_t N>
	void set( const char (&c)[N] )
	{
		static_assert( N == 5, "fourcc must be 4 characters" );
		_char[0] = c[0];
		_char[1] = c[1];
		_char[2] = c[2];
		_char[3] = c[3];
	}

	void set( uint32_t v )
	{
		_code = v;
	}

	/// @brief Compare FourCC with literal string.
	template<size_t N>
	bool operator==( const char (&c)[N] ) const
	{
		static_assert( N == 5, "fourcc must be 4 characters" );
		return c[0] == _char[0] && c[1] == _char[1] && c[2] == _char[2] && c[3] == _char[3];
	}

	/// @brief Compare FourCC for equality.
	bool operator==( const fourcc &c ) const
	{
		return _code == c._code;
	}

	/// @brief Compare FourCC for ordering.
	///
	/// This is mostly so FourCC can be used as keys in std::map and std::set.
	bool operator<( const fourcc &c ) const
	{
		return _code < c._code;
	}

	/// @brief Get the ith character.
	char operator[]( size_t i ) const
	{
		return _char[i];
	}

	/// @brief Get the ith character.
	char &operator[]( size_t i )
	{
		return _char[i];
	}

	/// @brief Test is the FourCC is valid.
	explicit operator bool() const
	{
		return _code != 0;
	}

private:
	union
	{
		char _char[4];
		uint32_t _code = 0;
	};
};

////////////////////////////////////////

/// @brief Output operator for FourCC.
std::ostream &operator<<( std::ostream &out, const fourcc &c );

/// @brief Input operator for FourCC.
std::istream &operator>>( std::istream &out, fourcc &c );

////////////////////////////////////////

}

