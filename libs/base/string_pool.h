// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstring>
#include <string>
#include "concurrent_mem_pool.h"

////////////////////////////////////////

namespace base
{

class string_pool;

class size_tagged_string
{
public:
	size_tagged_string( void ) = default;
	size_tagged_string( const char *ptr ) : _str( ptr ) {}

	size_t size( void ) const
	{
		signed char sz = reinterpret_cast<const signed char *>(_str)[-1];
		switch (sz)
		{
			case -8:
				return static_cast<size_t>(
					*(reinterpret_cast<const uint64_t *>(_str - 9)) );
			case -4:
				return static_cast<size_t>(
					*(reinterpret_cast<const uint32_t *>(_str - 5)) );
			case -2:
				return static_cast<size_t>(
					*(reinterpret_cast<const uint16_t *>(_str - 3)) );
			case -1:
				return static_cast<size_t>(
					*(reinterpret_cast<const uint8_t *>(_str - 2)) );
				break;
			default:
				return static_cast<size_t>( sz );
		}
	}
	bool empty( void ) const { return _str == nullptr; }
	const char *c_str( void ) const { return _str; }

	static size_tagged_string create( string_pool &p, const char *src, size_t len );

private:
	const char *_str = nullptr;
};

class string_pool
{
public:
	~string_pool( void ) = default;
	string_pool( const string_pool & ) = delete;
	string_pool &operator=( const string_pool & ) = delete;
	string_pool( string_pool && ) = delete;
	string_pool &operator=( string_pool && ) = delete;

	size_tagged_string create( const char *s )
	{
		return size_tagged_string::create( *this, s, std::strlen( s ) );
	}

	size_tagged_string create( const std::string &s )
	{
		return size_tagged_string::create( *this, s.c_str(), s.size() );
	}

	size_tagged_string create( const char *s, const char *end )
	{
		return size_tagged_string::create( *this, s, static_cast<size_t>( end - s ) );
	}

	char *alloc( size_t len );

	/// NB: assumes all size_tagged_string objects are gone
	/// that refer to this string pool
	std::pair<size_t, size_t> clear( void )
	{
		return _strings.clear();
	}

private:
	concurrent_mem_pool<1> _strings;
};

} // namespace base

