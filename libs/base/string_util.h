// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "const_string.h"
#include "likely.h"
#include "string_split.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

namespace base
{
////////////////////////////////////////

std::string trim( const std::string &str, const std::string &ws = " \t\n\r" );
std::string ltrim( const std::string &str, const std::string &ws = " \t\n\r" );
std::string rtrim( const std::string &str, const std::string &ws = " \t\n\r" );

////////////////////////////////////////

// TODO: generalize to arbitrary string types...
std::string
            replace( std::string &&str, char c, const std::string &replacement );
std::string replace( std::string &&str, char c, const cstring &replacement );
template <
    typename S,
    typename = typename std::enable_if<
        std::is_same<
            typename std::decay<typename std::remove_reference<S>::type>::type,
            char *>::value ||
            std::is_same<
                typename std::decay<
                    typename std::remove_reference<S>::type>::type,
                const char *>::value,
        int>::type>
inline std::string replace( std::string &&str, char c, S &&replacement )
{
    return replace(
        std::move( str ), c, cstring( std::forward<S>( replacement ) ) );
}

inline std::string
replace( std::string &&str, std::initializer_list<std::pair<char, cstring>> r )
{
    std::string ret = std::move( str );
    for ( auto &cr: r )
        ret = replace( std::move( ret ), cr.first, cr.second );
    return ret;
}
inline std::string replace(
    const std::string &str, std::initializer_list<std::pair<char, cstring>> r )
{
    return replace( std::string( str ), r );
}

////////////////////////////////////////

template <typename T> inline std::string to_string( const T &t )
{
    std::stringstream tmp;
    tmp << t;
    return tmp.str();
}

////////////////////////////////////////

constexpr inline int from_digit( char v ) { return int( v ) - int( '0' ); }

////////////////////////////////////////

inline int from_hex( char v )
{
    return ( std::isalpha( int( v ) ) != 0 )
               ? 10 + ( std::tolower( int( v ) ) - int( 'a' ) )
               : ( int( v ) - int( '0' ) );
}

inline char from_hex( char p1, char p2 )
{
    return static_cast<char>( from_hex( p1 ) * 16 + from_hex( p2 ) );
}

////////////////////////////////////////

constexpr inline char to_hex( int c )
{
    return static_cast<char>(
        ( c & 0xF ) >= 10 ? int( 'A' ) + ( ( c & 0xF ) - 10 )
                          : int( '0' ) + ( c & 0xF ) );
}
inline void to_hex( char &c1, char &c2, int v )
{
    c1 = to_hex( ( v & 0xF0 ) >> 4 );
    c2 = to_hex( ( v & 0x0F ) );
}

////////////////////////////////////////

template <typename stringT>
inline bool begins_with(
    const stringT &s,
    const const_string<
        typename stringT::value_type,
        typename stringT::traits_type> &start )
{
    if ( GK_UNLIKELY( start.empty() ) )
        return true;

    // need this one for begins_with with a static char * second arg
    return s.compare( 0, start.size(), start.data(), start.size() ) == 0;
}

// need to have this one to avoid an extra strlen call since specific
// template rules will pick the above version for 2 const_strings because
// b is more specific
template <typename charT, typename traitsT>
constexpr inline bool begins_with(
    const const_string<charT, traitsT> &s,
    const const_string<charT, traitsT> &start )
{
    return ( GK_UNLIKELY( start.empty() ) )
               ? true
               : ( s.compare( 0, start.size(), start, 0, start.size() ) == 0 );
}

////////////////////////////////////////

template <typename CharT, typename TraitsT>
inline bool ends_with(
    const std::basic_string<CharT, TraitsT> &s,
    const std::basic_string<CharT, TraitsT> &end )
{
    if ( GK_UNLIKELY( end.empty() ) )
        return true;

    if ( GK_UNLIKELY( s.size() < end.size() ) )
        return false;

    return s.compare( s.size() - end.size(), end.size(), end ) == 0;
}

template <typename CharT, typename TraitsT>
inline bool ends_with(
    const std::basic_string<CharT, TraitsT> &s,
    const CharT *endS )
{
    const_string<CharT, TraitsT> end( endS );
    if ( GK_UNLIKELY( end.empty() ) )
        return true;

    if ( GK_UNLIKELY( s.size() < end.size() ) )
        return false;

    return s.compare(
               s.size() - end.size(), end.size(), end.data(), end.size() ) == 0;
}

template <typename CharT, typename TraitsT>
constexpr inline bool ends_with(
    const const_string<CharT, TraitsT> &s,
    const const_string<CharT, TraitsT> &end )
{
    return ( GK_UNLIKELY( end.empty() ) )
               ? true
               : ( GK_UNLIKELY( s.size() < end.size() ) )
                     ? false
                     : ( s.compare( s.size() - end.size(), end.size(), end ) ==
                         0 );
}

template <typename CharT, typename TraitsT>
constexpr inline bool ends_with(
    const const_string<CharT, TraitsT> &s,
    const CharT *endS )
{
    return ends_with( s, const_string<CharT, TraitsT>( endS ) );
}

////////////////////////////////////////

constexpr size_t length( const const_string<char> &s ) { return s.size(); }

////////////////////////////////////////

/// @brief locale-specific comparison of strings
template <typename stringT>
inline int collate(
    const stringT &a, const stringT &b, const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::collate<typename stringT::value_type>>( loc );
    return f.compare(
        a.data(), a.data() + a.size(), b.data(), b.data() + b.size() );
}

template <typename stringT>
inline int collate(
    const stringT &a,
    const const_string<
        typename stringT::value_type,
        typename stringT::traits_type> &b,
    const std::locale &                 loc = std::locale() )
{
    auto &f = std::use_facet<std::collate<typename stringT::value_type>>( loc );
    return f.compare(
        a.data(), a.data() + a.size(), b.data(), b.data() + b.size() );
}

// TODO: can we do this without the string duplication? Seems like it should
// be possible if we subclass std::collate...
template <typename charT, typename traitsT, typename allocT>
inline int icollate(
    std::basic_string<charT, traitsT, allocT> a,
    std::basic_string<charT, traitsT, allocT> b,
    const std::locale &                       loc = std::locale() )
{
    auto &f = std::use_facet<std::ctype<charT>>( loc );
    std::transform( a.begin(), a.end(), a.begin(), [&]( charT v ) {
        return f.tolower( v );
    } );
    std::transform( b.begin(), b.end(), b.begin(), [&]( charT v ) {
        return f.tolower( v );
    } );

    auto &cf  = std::use_facet<std::collate<charT>>( loc );
    int   ret = cf.compare(
        a.data(), a.data() + a.size(), b.data(), b.data() + b.size() );
    return ret;
}

template <typename charT, typename traitsT, typename allocT>
inline int icollate(
    std::basic_string<charT, traitsT, allocT> a,
    const const_string<charT, traitsT> &      b,
    const std::locale &                       loc = std::locale() )
{
    return icollate(
        std::move( a ),
        std::basic_string<charT, traitsT, allocT>( b.begin(), b.end() ),
        loc );
}

template <typename charT, typename traitsT, typename allocT>
inline int icollate(
    std::basic_string<charT, traitsT, allocT> a,
    const charT *                             b,
    const std::locale &                       loc = std::locale() )
{
    return icollate(
        std::move( a ), std::basic_string<charT, traitsT, allocT>( b ), loc );
}

////////////////////////////////////////

/// @brief locale-specific case insensitive comparison function that can be
///        used for sorting
template <typename stringT>
inline bool iless( const stringT &a, const stringT &b )
{
    return icollate( a, b ) < 0;
}

////////////////////////////////////////

template <typename stringT>
inline stringT
to_lower( const stringT &str, const std::locale &loc = std::locale() )
{
    auto &  f = std::use_facet<std::ctype<typename stringT::value_type>>( loc );
    stringT ret = str;
    std::transform(
        ret.begin(),
        ret.end(),
        ret.begin(),
        [&]( typename stringT::value_type v ) { return f.tolower( v ); } );
    return ret;
}

////////////////////////////////////////

template <typename stringT>
inline stringT
to_upper( const stringT &str, const std::locale &loc = std::locale() )
{
    auto &  f = std::use_facet<std::ctype<typename stringT::value_type>>( loc );
    stringT ret = str;
    std::transform(
        ret.begin(),
        ret.end(),
        ret.begin(),
        [&]( typename stringT::value_type v ) { return f.toupper( v ); } );
    return ret;
}

////////////////////////////////////////

} // namespace base
