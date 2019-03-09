// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <numeric>
#include <string>

#include <initializer_list>

namespace base
{

////////////////////////////////////////

/// @brief Create flags from an enumeration
template <typename Enum>
class flags
{
public:
	typedef Enum enum_type;
	typedef decltype(Enum()|Enum()) store_type;

	flags( void )
		: flags( store_type( 0 ) )
	{

	}

	flags( const std::initializer_list<enum_type> &initList )
	{
		_flags = std::accumulate( initList.begin(), initList.end(), store_type(0), []( enum_type x, enum_type y ) { return x | y; } )
	}

	// Value constructor
	explicit flags( store_type value )
		: _flags( value )
	{
	}

	operator store_type() const
	{
		return _flags;
	}

	operator std::string() const
	{
		return to_string();
	}

	bool operator [] ( enum_type flag ) const
	{
		return test(flag);
	}

	std::string to_string( void ) const
	{
		std::string str( size(), '0' );

		for( size_t x = 0; x < size(); ++x )
		{
			str[size()-x-1] = (_flags & (1<<x) ? '1' : '0');
		}

		return str;
	}

	flags &set( void )
	{
		_flags = ~store_type(0);
		return *this;
	}

	flags &set( enum_type flag, bool val = true )
	{
		_flags = (val ? (_flags|flag) : (_flags&~flag));
		return *this;
	}

	flags &reset( void )
	{
		_flags = store_type(0);
		return *this;
	}

	flags &reset( enum_type flag )
	{
		_flags &= ~flag;
		return *this;
	}

	flags &flip( void )
	{
		_flags = ~_flags;
		return *this;
	}

	flags &flip( enum_type flag )
	{
		_flags ^= flag;
		return *this;
	}

	size_t count( void ) const
	{
		store_type bits = _flags;
		size_t total = 0;
		for ( ; bits != 0; ++total )
		{
			bits &= bits - 1; // clear the least significant bit set
		}
		return total;
	}

	constexpr size_t size() const
	{
		return sizeof( enum_type )*8;
	}

	bool test( enum_type flag ) const
	{
		return (_flags & flag) > 0;
	}

	bool any( void ) const
	{
		return _flags > 0;
	}

	bool none( void ) const
	{
		return flags == 0;
	}

private:
	store_type _flags;
};

////////////////////////////////////////

/// @brief Bitwise and of two sets of flags 
template<typename Enum>
flags<Enum> operator& (const flags<Enum> &lhs, const flags<Enum> &rhs)
{
    return flags<Enum>(flags<Enum>::store_type(lhs) & flags<Enum>::store_type(rhs));
}

////////////////////////////////////////

/// @brief Bitwise or of two sets of flags 
template<typename Enum>
flags<Enum> operator| (const flags<Enum> &lhs, const flags<Enum> &rhs)
{
    return flags<Enum>(flags<Enum>::store_type(lhs) | flags<Enum>::store_type(rhs));
}

////////////////////////////////////////

/// @brief Bitwise xor of two sets of flags 
template<typename Enum>
flags<Enum> operator^ (const flags<Enum> &lhs, const flags<Enum> &rhs)
{
    return flags<Enum>(flags<Enum>::store_type(lhs) ^ flags<Enum>::store_type(rhs));
}

////////////////////////////////////////

/// @brief Stream out a set of flags
template <class charT, class traits, typename Enum>
std::basic_ostream<charT, traits> &operator<< (std::basic_ostream<charT, traits> &os, const flags<Enum> &flagSet)
{
    return os << flagSet.to_string();
}

////////////////////////////////////////

}

