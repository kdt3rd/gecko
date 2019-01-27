// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "string_pool.h"

////////////////////////////////////////

namespace base
{

size_tagged_string
size_tagged_string::create( string_pool &pool, const char *src, size_t len )
{
	if ( len < 128 )
	{
		char *ptr = pool.alloc( len + 2 );
		ptr[0] = static_cast<char>( len );
		std::copy( src, src + len, ptr + 1 );
		ptr[len + 1] = '\0';
		return size_tagged_string( ptr + 1 );
	}

	if ( len < 256 )
	{
		char *ptr = pool.alloc( len + 3 );
		*(reinterpret_cast<uint8_t *>(ptr)) = static_cast<uint8_t>( len );
		++ptr;
		*ptr++ = static_cast<char>( -1 );
		std::copy( src, src + len, ptr );
		ptr[len] = '\0';
		return size_tagged_string( ptr );
	}

	if ( len < 65536 )
	{
		char *ptr = pool.alloc( len + 4 );
		*(reinterpret_cast<uint16_t *>(ptr)) = static_cast<uint16_t>( len );
		ptr += 2;
		*ptr++ = static_cast<char>( -2 );
		std::copy( src, src + len, ptr );
		ptr[len] = '\0';
		return size_tagged_string( ptr );
	}

	if ( len > std::numeric_limits<uint32_t>::max() )
	{
		char *ptr = pool.alloc( len + 10 );
		*(reinterpret_cast<uint64_t *>(ptr)) = static_cast<uint64_t>( len );
		ptr += 8;
		*ptr++ = static_cast<char>( -8 );
		std::copy( src, src + len, ptr );
		ptr[len] = '\0';
		return size_tagged_string( ptr );
	}

	char *ptr = pool.alloc( len + 6 );
	*(reinterpret_cast<uint32_t *>(ptr)) = static_cast<uint32_t>( len );
	ptr += 4;
	*ptr++ = static_cast<char>( -4 );
	std::copy( src, src + len, ptr );
	ptr[len] = '\0';
	return size_tagged_string( ptr );
}

////////////////////////////////////////

char *string_pool::alloc( size_t len )
{
	return _strings.get<char>( len );
}

////////////////////////////////////////

} // namespace base

