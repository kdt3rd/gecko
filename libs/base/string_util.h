
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <cstring>
#include <cctype>
#include <locale>
#include <algorithm>

#include "const_string.h"
#include "string_split.h"

namespace base
{

////////////////////////////////////////

std::string trim( const std::string &str, const std::string &ws = " \t\n\r" );
std::string ltrim( const std::string &str, const std::string &ws = " \t\n\r" );
std::string rtrim( std::string str, const std::string &ws = " \t\n\r" );


////////////////////////////////////////


std::string replace( std::string &&str, char c, const std::string &replacement );

inline std::string replace( const std::string &str, char c, const std::string &replacement )
{
	return replace( std::string( str ), c, replacement );
}

////////////////////////////////////////

template<typename T>
std::string to_string( const T &t )
{
	std::stringstream tmp;
	tmp << t;
	return tmp.str();
}

////////////////////////////////////////

template <typename stringT>
inline bool begins_with( const stringT &s,
						 const stringT &start )
{
	if ( start.empty() )
		return true;

	return s.compare( 0, start.size(), start, 0, start.size() ) == 0;
}

template <typename stringT>
inline bool begins_with( const stringT &s,
						 const const_string<typename stringT::value_type, typename stringT::traits_type> &start )
{
	if ( start.empty() )
		return true;

	// need this one for begins_with with a static char * second arg
	return s.compare( 0, start.size(), start.data(), start.size() ) == 0;
}

template <typename charT, typename traitsT>
inline bool begins_with( const const_string<charT, traitsT> &s,
						 const const_string<charT, traitsT> &start )
{
	if ( start.empty() )
		return true;

	// need to have this one to avoid an extra strlen call since specific
	// template rules will pick the above version for 2 const_strings because
	// b is more specific
	return s.compare( 0, start.size(), start, 0, start.size() ) == 0;
}


////////////////////////////////////////

template <typename stringT>
inline bool ends_with( const stringT &s,
					   const stringT &end )
{
	if ( end.empty() )
		return true;

	if ( s.size() < end.size() )
		return false;

	return s.compare( s.size() - end.size(), end.size(), end ) == 0;
}

template <typename stringT>
inline bool ends_with( const stringT &s,
					   const const_string<typename stringT::value_type, typename stringT::traits_type> &end )
{
	if ( end.empty() )
		return true;

	if ( s.size() < end.size() )
		return false;

	return s.compare( s.size() - end.size(), end.size(), end.data(), end.size() ) == 0;
}

template <typename CharT, typename TraitsT>
inline bool ends_with( const const_string<CharT, TraitsT> &s,
					   const const_string<CharT, TraitsT> &end )
{
	if ( end.empty() )
		return true;

	if ( s.size() < end.size() )
		return false;

	return s.compare( s.size() - end.size(), end.size(), end ) == 0;
}

////////////////////////////////////////

constexpr size_t length( const const_string<char> &s )
{
	return s.size();
}


////////////////////////////////////////


/// @brief locale-specific comparison of strings
template <typename stringT>
inline int collate( const stringT &a, const stringT &b, const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::collate<typename stringT::value_type>>( loc );
	return f.compare( a.data(), a.data() + a.size(),
					  b.data(), b.data() + b.size() );
}

template <typename stringT>
inline int collate( const stringT &a,
					const const_string<typename stringT::value_type, typename stringT::traits_type> &b,
					const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::collate<typename stringT::value_type>>( loc );
	return f.compare( a.data(), a.data() + a.size(),
					  b.data(), b.data() + b.size() );
}

/// @brief locale-specific case insensitive comparison
template <typename charT, typename traitsT, typename allocT>
inline int icollate( std::basic_string<charT, traitsT, allocT> a,
					 std::basic_string<charT, traitsT, allocT> b,
					 const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::ctype<charT>>(loc);
	std::transform( a.begin(), a.end(), a.begin(),
					[&](charT v) { return f.tolower( v ); } );
	std::transform( b.begin(), b.end(), b.begin(),
					[&](charT v) { return f.tolower( v ); } );

    auto &cf = std::use_facet<std::collate<charT>>( loc );
	int ret = cf.compare( a.data(), a.data() + a.size(),
						  b.data(), b.data() + b.size() );
	return ret;
}

template <typename charT, typename traitsT, typename allocT, std::size_t Bsz>
inline int icollate( std::basic_string<charT, traitsT, allocT> a,
					 const charT (&b)[Bsz],
					 const std::locale &loc = std::locale() )
{
	return icollate( std::move( a ), std::basic_string<charT, traitsT, allocT>( b, Bsz - 1 ), loc );
}

template <typename charT, typename traitsT, typename allocT, std::size_t Bsz>
inline int icollate( std::basic_string<charT, traitsT, allocT> a,
					 const charT *b,
					 const std::locale &loc = std::locale() )
{
	return icollate( std::move( a ), std::basic_string<charT, traitsT, allocT>( b ), loc );
}


////////////////////////////////////////


/// @brief locale-specific case insensitive comparison function that can be
///        used for sorting
template <typename stringT>
inline bool
iless( const stringT &a, const stringT &b )
{
	return icollate( a, b ) < 0;
}


////////////////////////////////////////


template <typename stringT>
inline stringT to_lower( const stringT &str, const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::ctype<typename stringT::value_type>>(loc);
	stringT ret = str;
	std::transform( ret.begin(), ret.end(), ret.begin(),
					[&](typename stringT::value_type v) { return f.tolower( v ); } );
	return std::move( ret );
}


////////////////////////////////////////


template <typename stringT>
inline stringT to_upper( const stringT &str, const std::locale &loc = std::locale() )
{
    auto &f = std::use_facet<std::ctype<typename stringT::value_type>>(loc);
	stringT ret = str;
	std::transform( ret.begin(), ret.end(), ret.begin(),
					[&](typename stringT::value_type v) { return f.toupper( v ); } );
	return std::move( ret );
}

////////////////////////////////////////

} // namespace base

