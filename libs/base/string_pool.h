// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "concurrent_mem_pool.h"

#include "const_string.h"

#include "fnv_hash.h"

#include <cstring>
#include <ostream>
#include <string>
#include <shared_mutex>
#include <unordered_map>

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
        if ( empty() )
            return 0;

        signed char sz = reinterpret_cast<const signed char *>( _str )[-1];
        switch ( sz )
        {
            case -8: // errr, yeah, big
                return static_cast<size_t>(
                    *( reinterpret_cast<const uint64_t *>( _str - 9 ) ) );
            case -4: // 65536-(2^32 - 1)
                return static_cast<size_t>(
                    *( reinterpret_cast<const uint32_t *>( _str - 5 ) ) );
            case -2: // 256-65535
                return static_cast<size_t>(
                    *( reinterpret_cast<const uint16_t *>( _str - 3 ) ) );
            case -1: // 128-255
                return static_cast<size_t>(
                    *( reinterpret_cast<const uint8_t *>( _str - 2 ) ) );
            // 0-127
            default: return static_cast<size_t>( sz );
        }
    }

    bool        empty( void ) const { return _str == nullptr; }
    const char *c_str( void ) const { return _str; }
    char        operator[]( size_t i ) const { return _str[i]; }

    explicit operator std::string( void ) const
    {
        return std::string( c_str(), size() );
    }

private:
    const char *_str = nullptr;
};

////////////////////////////////////////

inline bool
operator==( const size_tagged_string &a, const size_tagged_string &b )
{
    if ( a.c_str() == b.c_str() )
        return true;

    return a.size() == b.size() &&
           std::memcmp( a.c_str(), b.c_str(), a.size() * sizeof( char ) ) == 0;
}
inline bool operator==( const char *s, const size_tagged_string &sts )
{
    return 0 == strcmp( sts.c_str(), s );
}
inline bool operator==( const size_tagged_string &sts, const char *s )
{
    return s == sts;
}
inline bool operator==( const std::string &a, const size_tagged_string &b )
{
    return a.size() == b.size() &&
           std::memcmp( a.c_str(), b.c_str(), a.size() * sizeof( char ) ) == 0;
}
inline bool operator==( const size_tagged_string &sts, const std::string &s )
{
    return s == sts;
}

////////////////////////////////////////

inline bool
operator!=( const size_tagged_string &a, const size_tagged_string &b )
{
    return !( a == b );
}
inline bool operator!=( const char *s, const size_tagged_string &sts )
{
    return !( s == sts );
}
inline bool operator!=( const size_tagged_string &sts, const char *s )
{
    return s != sts;
}
inline bool operator!=( const std::string &s, const size_tagged_string &sts )
{
    return !( s == sts );
}
inline bool operator!=( const size_tagged_string &sts, const std::string &s )
{
    return s != sts;
}

////////////////////////////////////////

inline bool
operator<( const size_tagged_string &a, const size_tagged_string &b )
{
    if ( a.c_str() == b.c_str() )
        return false;
    return a.size() < b.size() ||
           ( a.size() == b.size() &&
             std::memcmp( a.c_str(), b.c_str(), a.size() * sizeof( char ) ) <
                 0 );
}
inline bool operator<( const size_tagged_string &a, const std::string &b )
{
    return a.size() < b.size() ||
           ( a.size() == b.size() &&
             std::memcmp( a.c_str(), b.c_str(), a.size() * sizeof( char ) ) <
                 0 );
}
inline bool operator<( const std::string &a, const size_tagged_string &b )
{
    return a.size() < b.size() ||
           ( a.size() == b.size() &&
             std::memcmp( a.c_str(), b.c_str(), a.size() * sizeof( char ) ) <
                 0 );
}
inline bool operator<( const size_tagged_string &a, const char *b )
{
    size_t lenb = strlen( b );
    return a.size() < lenb ||
           ( a.size() == lenb &&
             std::memcmp( a.c_str(), b, lenb * sizeof( char ) ) < 0 );
}
inline bool operator<( const char *a, const size_tagged_string &b )
{
    size_t lena = strlen( a );
    return lena < b.size() ||
           ( lena == b.size() &&
             std::memcmp( a, b.c_str(), lena * sizeof( char ) ) < 0 );
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const size_tagged_string &s )
{
    os << s.c_str();
    return os;
}

template <typename T>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const size_tagged_string &v )
{
    h.add( v.c_str(), v.size() * sizeof( char ) );
    return h;
}

} // namespace base

namespace std
{
// overload std::hash
template <> struct hash<base::size_tagged_string>
{
    typedef base::size_tagged_string argument_type;
    typedef std::size_t              result_type;
    inline result_type operator()( argument_type const &s ) const noexcept
    {
        base::fnv1a<result_type> hfunc;
        hfunc.add( s.c_str(), s.size() * sizeof( char ) );
        return static_cast<result_type>( hfunc.hash() );
    }
};
} // namespace std

namespace base
{
////////////////////////////////////////////////////////////////////////////////

class string_pool
{
public:
    ~string_pool( void )               = default;
    string_pool( const string_pool & ) = delete;
    string_pool &operator=( const string_pool & ) = delete;
    string_pool( string_pool && )                 = delete;
    string_pool &operator=( string_pool && ) = delete;

    size_tagged_string create( const cstring &s );
    size_tagged_string create( const char *s, const char *end );

    /// NB: assumes all size_tagged_string objects are gone
    /// that refer to this string pool
    std::pair<size_t, size_t> clear( void );

private:
    char *             alloc( size_t len );
    size_tagged_string make( const char *src, size_t len );
    size_tagged_string add( const cstring &s );

    concurrent_mem_pool<1>                          _strings;
    std::shared_timed_mutex                         _mutex;
    std::unordered_map<cstring, size_tagged_string> _pre_created;
};

} // namespace base
