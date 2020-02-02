// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "const_string.h"

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>
#include <functional>

////////////////////////////////////////

namespace base
{
namespace detail
{
template <typename T> struct fnv1a_impl
{};

template <> struct fnv1a_impl<uint32_t>
{
    static const uint32_t basis = 2166136261UL;
    static const uint32_t prime = 16777619UL;

    static inline constexpr uint32_t add( uint32_t h, uint8_t v ) noexcept
    {
        return ( h ^ static_cast<uint32_t>( v ) ) * prime;
    }

    template <std::size_t N>
    static inline constexpr uint32_t
    add_n( uint32_t h, const uint8_t *v ) noexcept
    {
        return N == 0 ? h
                      : add( add_n<( N > 0 ? N - 1 : N )>( h, v ), v[N - 1] );
    }

    template <std::size_t N>
    static inline constexpr uint32_t build( const uint8_t *v ) noexcept
    {
        return add_n<N>( basis, v );
    }
};

template <> struct fnv1a_impl<uint64_t>
{
    static const uint64_t basis = 14695981039346656037ULL;
    static const uint64_t prime = 1099511628211ULL;

    static inline constexpr uint64_t add( uint64_t h, uint8_t v ) noexcept
    {
        return ( h ^ static_cast<uint64_t>( v ) ) * prime;
    }

    template <std::size_t N>
    static inline constexpr uint64_t
    add_n( uint64_t h, const uint8_t *v ) noexcept
    {
        return N == 0 ? h
                      : add( add_n<( N > 0 ? N - 1 : N )>( h, v ), v[N - 1] );
    }

    template <std::size_t N>
    static inline constexpr uint64_t build( const uint8_t *v ) noexcept
    {
        return add_n<N>( basis, v );
    }
};

} // namespace detail

///
/// @brief Class fnv1a provides an implementation of the fnv 1-a hash.
///
/// The 1-a version provides better avalanche behavior over the v.1
/// algorithm.
///
/// NB: this only allows unsigned integer representations, as that is
/// how the algorithm is defined. Some implementations allow signed or
/// unsigned, but the values are different, and unclear the
/// implications.
///
/// other hash primes and offsets are defined for different sizes but
/// those are not currently supported...
///
template <
    typename T,
    typename = typename std::enable_if<
        std::is_integral<T>::value && std::is_unsigned<T>::value &&
            ( sizeof( T ) == 4 || sizeof( T ) == 8 ),
        T>::type>
class fnv1a
{
    using hash_type = typename std::conditional<
        sizeof( T ) == 4,
        uint32_t,
        typename std::conditional<sizeof( T ) == 8, uint64_t, void>::type>::
        type;
    using fnv_impl               = detail::fnv1a_impl<hash_type>;
    static const hash_type basis = fnv_impl::basis;
    static const hash_type prime = fnv_impl::prime;

public:
    using value_type = T;

    constexpr fnv1a( void ) noexcept          = default;
    ~fnv1a( void )                            = default;
    constexpr fnv1a( const fnv1a & ) noexcept = default;
    constexpr fnv1a( fnv1a && ) noexcept      = default;
    fnv1a &operator=( const fnv1a & ) = default;
    fnv1a &operator=( fnv1a && ) = default;

    template <size_t N> inline void add( const uint8_t *v )
    {
        _hash = fnv_impl::template add_n<N>( _hash, v );
    }

    inline void add( const void *m, size_t len )
    {
        const uint8_t *octets = static_cast<const uint8_t *>( m );
        for ( size_t i = 0; i != len; ++i )
            _hash = fnv_impl::add( _hash, octets[i] );
    }

    constexpr value_type operator()( void ) const
    {
        return static_cast<value_type>( _hash );
    }

    constexpr value_type hash( void ) const { return _hash; }

    template <size_t N>
    constexpr static inline fnv1a compute( const uint8_t *v ) noexcept
    {
        return fnv1a( fnv_impl::build<N>( v ) );
    }

private:
    constexpr explicit inline fnv1a( hash_type x ) : _hash( x ) {}
    hash_type _hash = basis;
};

template <typename T, typename V> fnv1a<T> &operator<<( fnv1a<T> &h, V &v )
{
    h.template add<sizeof( V )>(
        reinterpret_cast<const uint8_t *>( std::addressof( v ) ) );
    return h;
}

template <
    typename T,
    typename charT,
    typename traitsT = std::char_traits<charT>,
    typename allocT  = std::allocator<charT>>
inline fnv1a<T> &
operator<<( fnv1a<T> &h, const std::basic_string<charT, traitsT, allocT> &v )
{
    h.add( v.c_str(), v.size() * sizeof( charT ) );
    return h;
}

template <
    typename T,
    typename charT,
    typename traitsT = std::char_traits<charT>>
inline fnv1a<T> &
operator<<( fnv1a<T> &h, const const_string<charT, traitsT> &v )
{
    h.add( v.c_str(), v.size() * sizeof( charT ) );
    return h;
}

template <typename T> inline fnv1a<T> &operator<<( fnv1a<T> &h, const char *s )
{
    h.add( s, std::char_traits<char>::length( s ) );
    return h;
}

// hrm, hashing w/ hash values, how meta... be careful doing this -
// check the mixing
template <typename T, typename Tx>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const fnv1a<Tx> &x )
{
    h << x();
    return h;
}

template <typename T, typename V>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::vector<V> &v )
{
    for ( auto &e: v )
        h << e;
    return h;
}

template <typename T, typename V, size_t N>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::array<V, N> &v )
{
    for ( auto &e: v )
        h << e;
    return h;
}

template <typename T, typename A, typename B>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::pair<A, B> &v )
{
    return h << v.first << v.second;
}

namespace detail
{
template <std::size_t I = 0, typename T, typename... Tp>
inline typename std::enable_if<I == sizeof...( Tp ), void>::type
tuple_fnv_loop( fnv1a<T> &, const std::tuple<Tp...> & )
{}
template <std::size_t I = 0, typename T, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...( Tp ), void>::type
    tuple_fnv_loop( fnv1a<T> &h, const std::tuple<Tp...> &t )
{
    h << std::get<I>( t );
    tuple_fnv_loop<I + 1, Tp...>( h, t );
}

} // namespace detail

template <typename T, typename... Args>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::tuple<Args...> &v )
{
    // TODO: revisit this if we end up requiring c++14 or newer
    detail::tuple_fnv_loop( h, v );
}

template <typename T, typename K, typename V>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::map<K, V> &v )
{
    for ( auto &e: v )
        h << e;
    return h;
}

template <typename T, typename V>
inline fnv1a<T> &operator<<( fnv1a<T> &h, const std::shared_ptr<V> &v )
{
    return h << *v;
}

template <typename T>
std::ostream &operator<<( std::ostream &os, const fnv1a<T> &h )
{
    os << "0x" << std::hex << h() << std::dec;
    return os;
}

////////////////////////////////////////
// enable use as a map key

template <typename T>
inline bool operator==( const fnv1a<T> &a, const fnv1a<T> &b )
{
    return a() == b();
}
template <typename T>
inline bool operator!=( const fnv1a<T> &a, const fnv1a<T> &b )
{
    return a() != b();
}
template <typename T>
inline bool operator<( const fnv1a<T> &a, const fnv1a<T> &b )
{
    return a() < b();
}

using fnv1a_32 = fnv1a<uint32_t>;
using fnv1a_64 = fnv1a<uint64_t>;

} // namespace base

namespace std
{
// overload std::hash
template <typename charT, typename traitsT>
struct hash<base::const_string<charT, traitsT>>
{
    typedef base::const_string<charT, traitsT> argument_type;
    typedef std::size_t                        result_type;
    inline result_type operator()( argument_type const &s ) const noexcept
    {
        base::fnv1a<result_type> hfunc;
        hfunc.add( s.c_str(), s.size() * sizeof( char ) );
        return static_cast<result_type>( hfunc.hash() );
    }
};
} // namespace std
