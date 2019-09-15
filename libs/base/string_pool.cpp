// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "string_pool.h"

#include <limits>

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

size_tagged_string string_pool::create( const cstring &s )
{
    {
        std::shared_lock<std::shared_timed_mutex> lk( _mutex );
        auto i = _pre_created.find( s );
        if ( i != _pre_created.end() )
            return i->second;
    }
    return add( s );
}

size_tagged_string string_pool::create( const char *s, const char *end )
{
    return create( cstring( s, end ) );
}

std::pair<size_t, size_t> string_pool::clear( void )
{
    std::lock_guard<std::shared_timed_mutex> lk( _mutex );
    _pre_created.clear();
    return _strings.clear();
}

////////////////////////////////////////

char *string_pool::alloc( size_t len ) { return _strings.get<char>( len ); }

////////////////////////////////////////

size_tagged_string
string_pool::make( const char *src, size_t len )
{
    if ( len < 128 )
    {
        char *ptr = alloc( len + 2 );
        ptr[0]    = static_cast<char>( len );
        std::copy( src, src + len, ptr + 1 );
        ptr[len + 1] = '\0';
        return size_tagged_string( ptr + 1 );
    }

    if ( len < 256 )
    {
        char *ptr                               = alloc( len + 3 );
        *( reinterpret_cast<uint8_t *>( ptr ) ) = static_cast<uint8_t>( len );
        ++ptr;
        *ptr++ = static_cast<char>( -1 );
        std::copy( src, src + len, ptr );
        ptr[len] = '\0';
        return size_tagged_string( ptr );
    }

    if ( len < 65536 )
    {
        char *ptr                                = alloc( len + 4 );
        *( reinterpret_cast<uint16_t *>( ptr ) ) = static_cast<uint16_t>( len );
        ptr += 2;
        *ptr++ = static_cast<char>( -2 );
        std::copy( src, src + len, ptr );
        ptr[len] = '\0';
        return size_tagged_string( ptr );
    }

    if ( len > std::numeric_limits<uint32_t>::max() )
    {
        char *ptr                                = alloc( len + 10 );
        *( reinterpret_cast<uint64_t *>( ptr ) ) = static_cast<uint64_t>( len );
        ptr += 8;
        *ptr++ = static_cast<char>( -8 );
        std::copy( src, src + len, ptr );
        ptr[len] = '\0';
        return size_tagged_string( ptr );
    }

    char *ptr                                = alloc( len + 6 );
    *( reinterpret_cast<uint32_t *>( ptr ) ) = static_cast<uint32_t>( len );
    ptr += 4;
    *ptr++ = static_cast<char>( -4 );
    std::copy( src, src + len, ptr );
    ptr[len] = '\0';
    return size_tagged_string( ptr );
}

////////////////////////////////////////

size_tagged_string string_pool::add( const cstring &s )
{
    std::lock_guard<std::shared_timed_mutex> lk( _mutex );
    // someone might have gotten here before us, avoid double creation
    auto i = _pre_created.find( s );
    if ( i != _pre_created.end() )
        return i->second;

    size_tagged_string n = make( s.c_str(), s.size() );
    _pre_created[cstring(n.c_str(), n.size())] = n;
    return n;
}
} // namespace base
