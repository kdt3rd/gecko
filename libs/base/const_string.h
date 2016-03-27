//
// Copyright (c) 2014 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <string>
#include <type_traits>
#include <ostream>
#include <cstring>
#include <limits>
#ifdef __has_include
# if (__cplusplus >= 201402L)
#  if __has_include(<string_view>)
#   include <string_view>
#   define ENABLE_STRING_VIEW_COMPAT
#  elif __has_include(<experimental/string_view>)
#   include <experimental/string_view>
#   define ENABLE_STRING_VIEW_COMPAT
#  endif
# endif
#endif


////////////////////////////////////////


namespace base
{

template < typename charT, typename traitsT = std::char_traits<charT> > class const_string;

typedef const_string<char> cstring;
typedef const_string<wchar_t> wcstring;
typedef const_string<char16_t> u16cstring;
typedef const_string<char32_t> u32cstring;

///
/// @brief Class const_string provides a simple memory reference string
///        class, primarily designed to wrap static (const) strings as a
///        class to use other algorithms without making a copy into a
///        basic_string.
///
/// NB: the string passed in MUST be in an outer scope of this object.
///     conststr is very lightweight, does not copy, and even things
///     like substr just update pointers, but don't modify the underlying
///     values, so printing a conststr is a bit harder than a normal
///     string.
///
/// TODO: C++17 defines string_view, need to merge this with that as
///       c++17 becomes the norm. for now, we just
///       provide transparent conversion to and from
template <class charT, typename traitsT>
class const_string
{
public:
	typedef traitsT traits_type;
	typedef typename traitsT::char_type value_type;
	typedef std::size_t size_type;
	typedef const value_type * const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	static const size_type npos = static_cast<size_type>(-1);

	constexpr const_string( void ) : _str( nullptr ), _sz( 0 ) {}
	template <std::size_t N>
	constexpr const_string( const value_type (&s)[N] ) : _str( s ), _sz( N - 1 ) {}
	constexpr const_string( const value_type *s, size_type N ) : _str( s ), _sz( N ) {}
	template <typename allocTs>
	constexpr const_string( const std::basic_string<charT, traitsT, allocTs> &s ) : _str( s.data() ), _sz( s.size() ) {}
	constexpr const_string( const value_type *s ) : _str( s ), _sz( traits_type::length( s ) ) {}

	constexpr const_string( const const_string & ) noexcept = default;
	constexpr const_string( const_string && ) noexcept = default;
	const_string &operator=( const const_string & ) noexcept = default;
	const_string &operator=( const_string && ) noexcept = default;

	/// @brief cast operator to std::basic_string
	constexpr operator std::basic_string<charT, traitsT>( void ) const { return to_string(); }
	/// @brief conversion function
	constexpr std::basic_string<charT, traitsT> to_string( void ) const { return empty() ? std::basic_string<charT, traitsT>() : std::basic_string<charT, traitsT>( data(), size() ); }

#ifdef ENABLE_STRING_VIEW_COMPAT
	constexpr const_string( const std::basic_string_view<charT, traitsT> &sv ) : _str( sv.data() ), _sz( sv.size() ) {}
	/// @brief cast operator to std::basic_string_view
	constexpr operator std::basic_string_view<charT, traitsT>( void ) const { return std::basic_string_view<charT, traitsT>( data(), size() ); }
#endif

	constexpr const value_type operator[]( size_type i ) const { return _str[i]; }
	constexpr const value_type at( size_type i ) const { return i <= _sz ? _str[i] : throw std::out_of_range( "const_string access out of range" ); }
	constexpr size_type size( void ) const { return _sz; }
	constexpr size_type length( void ) const { return _sz; }
	constexpr size_type max_size( void ) const noexcept
	{
		return ( npos - sizeof(size_type) - sizeof(void *) ) / sizeof(value_type) / 4;
	}

	constexpr bool empty( void ) const noexcept { return _sz == 0; }

	constexpr const_iterator begin( void ) const { return _str; }
	constexpr const_iterator end( void ) const { return _str + _sz + 1; }
	constexpr const_iterator cbegin( void ) const { return _str; }
	constexpr const_iterator cend( void ) const { return _str + _sz + 1; }

	constexpr const_reverse_iterator rbegin( void ) const { return const_reverse_iterator(end()); }
	constexpr const_reverse_iterator rend( void ) const { return const_reverse_iterator(begin()); }
	constexpr const_reverse_iterator crbegin( void ) const { return const_reverse_iterator(end()); }
	constexpr const_reverse_iterator crend( void ) const { return const_reverse_iterator(begin()); }

	constexpr const value_type &front( void ) const { return *_str; }
	constexpr const value_type &back( void ) const { return *(_str + ( size() - 1 ) ); }
	constexpr const value_type *data( void ) const noexcept { return _str; }

	void remove_prefix( size_type n )
	{
		_sz -= std::min( _sz, n );
		_str = ( (_sz == 0) ? nullptr : _str + n );
	}
	void remove_suffix( size_type n )
	{
		_sz -= std::min( _sz, n );
		_str = ( (_sz == 0) ? nullptr : _str );
	}

	constexpr int
	compare( const const_string &s ) const
	{
		return ( size() <= s.size() ?
				 compare_priv( begin(), s.begin(), size(), size() == s.size() ? 0 : -1 ) :
				 compare_priv( begin(), s.begin(), s.size(), 1 ) );
	}

	template <size_t N>
	constexpr int
	compare( const value_type (&s)[N] ) const
	{
		return ( size() <= (N-1) ?
				 compare_priv( begin(), s, size(), size() == (N-1) ? 0 : -1 ) :
				 compare_priv( begin(), s, (N-1), 1 ) );
	}

	template <typename allocT>
	constexpr int
	compare( const std::basic_string<value_type,traits_type, allocT> &s ) const
	{
		return ( size() <= s.size() ?
				 compare_priv( begin(), s.data(), size(), size() == s.size() ? 0 : -1 ) :
				 compare_priv( begin(), s.data(), s.size(), 1 ) );
	}

	constexpr int compare( size_type pos, size_type n, const const_string &s ) const
	{
		return substr( pos, n ).compare( s );
	}

	template <std::size_t N>
	constexpr int compare( size_type pos, size_type n, const value_type (&s)[N] ) const
	{
		return substr( pos, n ).compare( const_string( s, N - 1 ) );
	}

	constexpr int compare( size_type pos, size_type n, const value_type *s ) const
	{
		return substr( pos, n ).compare( const_string( s ) );
	}

	constexpr int compare( size_type pos1, size_type n1, const const_string &s, size_type pos2, size_type n2 ) const
	{
		return substr( pos1, n1 ).compare( s.substr( pos2, n2 ) );
	}

	constexpr int compare( size_type pos1, size_type n1, const value_type *s, size_type n2 ) const
	{
		return substr( pos1, n1 ).compare( const_string( s, n2 ) );
	}

	constexpr size_type find( value_type c, size_type pos = 0 ) const
	{
		return find_first_of( c, pos );
	}

	constexpr size_type find_first_of( value_type c, size_type pos = 0 ) const
	{
		return ( pos < size() ?
				 ( traits_type::eq( _str[pos], c ) ? pos : find_first_of( c, pos + 1 ) ) :
				 npos );
	}
	constexpr size_type find_first_of( const_string s, size_type pos = 0 ) const
	{
		return find_first_of( s.data(), pos, s.size() );
	}
	constexpr size_type find_first_of( const value_type *s, size_type pos, size_type n ) const
	{
		return ( pos < size() ?
				 ( traits_type::find( s, n, _str[pos] ) != nullptr ?
				   pos : find_first_of( s, pos + 1, n ) ) : npos );
	}


	constexpr size_type find_first_not_of( value_type c, size_type pos = 0 ) const
	{
		return ( pos < size() ?
				 ( ! traits_type::eq( _str[pos], c ) ? pos : find_first_not_of( c, pos + 1 ) ) :
				 npos );
	}

	constexpr size_type find_last_of( value_type c, size_type pos = npos ) const
	{
		return ( pos >= size() ? find_last_of( c, size() - 1 ) :
				 ( traits_type::eq( _str[pos], c ) ? pos :
				   ( pos == 0 ? npos : find_last_of( c, pos - 1 ) ) ) );
	}

	constexpr const_string substr( size_type pos = 0, size_type n = npos ) const
	{
		return ( pos <= _sz ?
				 const_string( _str + pos, ( n < _sz - pos ) ? n : _sz - pos ) :
				 throw std::out_of_range( "Invalid start position for constant string" ) );
	}

	std::basic_string<value_type, traits_type> str( void ) const
	{
		return std::basic_string<value_type, traits_type>( _str, _sz );
	}

private:
	constexpr int compare_priv( const_iterator a, const_iterator b, size_type n, int eqret ) const
	{
		return n == 0 ? eqret : ( traits_type::lt( *a, *b ) ? -1 : ( traits_type::lt( *b, *a ) ? 1 : compare_priv( a + 1, b + 1, n - 1, eqret ) ) );
	}

	const value_type *_str;
	size_type _sz;
};

template <typename streamT, typename charT, typename traitsT>
inline streamT &operator<<( streamT &os, const const_string<charT, traitsT> &s )
{
	os.write( s.begin(), s.size() );
	return os;
}


////////////////////////////////////////


template <typename charT, typename traitsT>
inline constexpr bool
operator==( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT>
inline constexpr bool
operator!=( const_string<charT, traitsT> lhs, const_string<charT, traitsT> rhs )
{
	return lhs.compare( rhs ) != 0;
}

template <typename charT, typename traitsT, std::size_t N>
inline constexpr bool
operator==( const const_string<charT, traitsT> &lhs, const charT (&rhs)[N] )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT, std::size_t N>
inline constexpr bool
operator==( const charT (&lhs)[N], const const_string<charT, traitsT> &rhs )
{
	return rhs.compare( lhs ) == 0;
}

template <typename charT, typename traitsT, typename allocT>
inline constexpr bool
operator==( const std::basic_string<charT, traitsT, allocT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return rhs.compare( lhs ) == 0;
}

template <typename charT, typename traitsT, typename allocT>
inline constexpr bool
operator==( const const_string<charT, traitsT> &lhs, const std::basic_string<charT, traitsT, allocT> &rhs )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT, std::size_t N>
inline constexpr bool
operator!=( const const_string<charT, traitsT> &lhs, const charT (&rhs)[N] )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT, std::size_t N>
inline constexpr bool
operator!=( const charT (&lhs)[N], const const_string<charT, traitsT> &rhs )
{
	return !( rhs == lhs );
}

template <typename charT, typename traitsT, typename allocT>
inline constexpr bool
operator!=( const std::basic_string<charT, traitsT, allocT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return !( rhs == lhs );
}

template <typename charT, typename traitsT, typename allocT>
inline constexpr bool
operator!=( const const_string<charT, traitsT> &lhs, const std::basic_string<charT, traitsT, allocT> &rhs )
{
	return !( lhs == rhs );
}


////////////////////////////////////////


template <typename charT, typename traitsT>
inline constexpr bool
operator<( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) < 0;
}

template <typename charT, typename traitsT>
inline constexpr bool
operator>( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) > 0;
}

template <typename charT, typename traitsT>
inline constexpr bool
operator<=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) <= 0;
}

template <typename charT, typename traitsT>
inline constexpr bool
operator>=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) >= 0;
}


////////////////////////////////////////


template <typename charT, typename traitsT, typename allocT>
inline typename std::basic_string<charT, traitsT, allocT> operator+( const std::basic_string<charT, traitsT, allocT> &s, const_string<charT, traitsT> cs )
{
	std::basic_string<charT, traitsT, allocT> ret = s;
	ret.append( cs.data(), cs.size() );
	return ret;
}

template <typename charT, typename traitsT, typename allocT>
inline typename std::basic_string<charT, traitsT, allocT> operator+( std::basic_string<charT, traitsT, allocT> &&s, const_string<charT, traitsT> cs )
{
	s.append( cs.data(), cs.size() );
	return std::move( s );
}

template <typename charT, typename traitsT>
inline typename std::basic_string<charT, traitsT> operator+( const_string<charT, traitsT> a, const_string<charT, traitsT> b )
{
	return a.to_string() + b;
}

////////////////////////////////////////

namespace detail
{
template <typename RetT, typename Ret = RetT, typename CharT, typename... Base>
inline Ret stoa( RetT (*convFunc)( const CharT *, CharT **, Base... ), const char *fname, const CharT *s, std::size_t *pos, Base... base )
{
	Ret ret;
	CharT *endP;
	errno = 0;
	const RetT tmp = convFunc( s, &endP, base... );
	if ( endP == s )
		throw std::invalid_argument( fname );
	else if ( errno == ERANGE ||
			  ( std::is_same<Ret,int>::value &&
				( tmp < static_cast<RetT>( std::numeric_limits<int>::min() ) ||
				  tmp > static_cast<RetT>( std::numeric_limits<int>::max() ) ) ) )
		throw std::out_of_range( fname );
	else
		ret = static_cast<Ret>( tmp );
	if ( pos )
		*pos = endP - s;

	return ret;
}

} //namespace detail


inline int stoi( cstring s, std::size_t *pos = 0, int base = 10 )
{
	return detail::stoa<long,int>( &std::strtol, "base::const_string::stoi", s.data(), pos, base );
}

inline long stol( cstring s, std::size_t *pos = 0, int base = 10 )
{
	return detail::stoa( &std::strtol, "base::const_string::stol", s.data(), pos, base );
}

inline long long stoll( cstring s, std::size_t *pos = 0, int base = 10 )
{
	return detail::stoa( &std::strtoll, "base::const_string::stoll", s.data(), pos, base );
}

inline unsigned long stoul( cstring s, std::size_t *pos = 0, int base = 10 )
{
	return detail::stoa( &std::strtoul, "base::const_string::stoul", s.data(), pos, base );
}

inline unsigned long long stoull( cstring s, std::size_t *pos = 0, int base = 10 )
{
	return detail::stoa( &std::strtoull, "base::const_string::stoull", s.data(), pos, base );
}

inline float stof( cstring s, std::size_t *pos = 0 )
{
	return detail::stoa( &std::strtof, "base::const_string::stof", s.data(), pos );
}
inline double stod( cstring s, std::size_t *pos = 0 )
{
	return detail::stoa( &std::strtod, "base::const_string::stod", s.data(), pos );
}
inline long double stold( cstring s, std::size_t *pos = 0 )
{
	return detail::stoa( &std::strtold, "base::const_string::stold", s.data(), pos );
}

} // namespace base




