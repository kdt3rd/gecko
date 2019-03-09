// Copyright (c) 2014-2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <type_traits>
#include <ostream>
#include <cstring>
#include <limits>
#include <stdexcept>

#ifdef __has_include
# if (__cplusplus > 201402L)
#  if __has_include(<string_view>)
#   include <string_view>
#   define ENABLE_STRING_VIEW_COMPAT
#   define STRING_VIEW_COMPAT_NS std
#  elif __has_include(<experimental/string_view>)
#   include <experimental/string_view>
#   define ENABLE_STRING_VIEW_COMPAT
#   define STRING_VIEW_COMPAT_NS std::experimental
#  endif
# endif
#endif


////////////////////////////////////////


namespace base
{

template < typename charT, typename traitsT = std::char_traits<charT> > class const_string;

using cstring = const_string<char>;
using wcstring = const_string<wchar_t>;
using u16cstring = const_string<char16_t>;
using u32cstring = const_string<char32_t>;

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
	template <typename S, bool = false>
	struct ctor_helper
	{
		template <typename V>
		static constexpr std::size_t length( const V *str )
		{
			return traitsT::length( str );
		}
	};
	template <typename S>
	struct ctor_helper<S, true>
	{
		template <typename V, std::size_t N>
		static constexpr std::size_t length( const V (&)[N] )
		{
			return N - 1;
		}
	};
	template <typename S>
	static constexpr std::size_t ctor_len( S &&s )
	{
		using Y = typename std::remove_reference<S>::type;
		using help = ctor_helper<Y, std::is_array<Y>::value && std::is_const<Y>::value>;
		return help::length( std::forward<S>( s ) );
	}

public:
	using traits_type = traitsT;
	using value_type = typename traitsT::char_type;
	using size_type = std::size_t;
	using const_iterator = const value_type *;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	static const size_type npos = static_cast<size_type>(-1);

	constexpr const_string( void ) noexcept = default;
	constexpr const_string( const value_type *s, size_type N ) noexcept : _str( s ), _sz( N ) {}
	template <typename allocTs>
	constexpr const_string( const std::basic_string<charT, traitsT, allocTs> &s ) noexcept // NOLINT
	: _str( s.data() ), _sz( s.size() ) {}
	/// NB: this will give probably unexpected results for values of
	/// const char buf[1024] = { "foo" };
	/// const_string foo( buf );
	/// -> the string length will be 1023
	/// however
	/// const char foo[] = { "foo" };
	/// will work as expected
	/// also
	/// char buf[50]
	/// should be correctly handled (and run strlen)
	template <typename S, typename = typename std::enable_if<std::is_same<typename std::decay<typename std::remove_reference<S>::type>::type, value_type *>::value ||
															 std::is_same<typename std::decay<typename std::remove_reference<S>::type>::type, const value_type *>::value, int>::type >
	constexpr const_string( S &&s ) noexcept // NOLINT
	: _str( s ), _sz( ctor_len( std::forward<S>( s ) ) ) {}

	~const_string( void ) = default;

	constexpr const_string( const const_string & ) noexcept = default;
	constexpr const_string( const_string && ) noexcept = default;
	const_string &operator=( const const_string & ) noexcept = default;
	const_string &operator=( const_string && ) noexcept = default; // NOLINT

	/// @brief cast operator to std::basic_string
	constexpr operator std::basic_string<charT, traitsT>( void ) const { return to_string(); } // NOLINT
	/// @brief conversion function
	constexpr std::basic_string<charT, traitsT> to_string( void ) const { return empty() ? std::basic_string<charT, traitsT>() : std::basic_string<charT, traitsT>( data(), size() ); }

#ifdef ENABLE_STRING_VIEW_COMPAT
	constexpr const_string( const STRING_VIEW_COMPAT_NS::basic_string_view<charT, traitsT> &sv ) : _str( sv.data() ), _sz( sv.size() ) {}
	/// @brief cast operator to std::basic_string_view
	constexpr operator STRING_VIEW_COMPAT_NS::basic_string_view<charT, traitsT>( void ) const { return STRING_VIEW_COMPAT_NS::basic_string_view<charT, traitsT>( data(), size() ); }
#endif

	constexpr const value_type operator[]( size_type i ) const { return _str[i]; }
	constexpr const value_type at( size_type i ) const { return i <= _sz ? _str[i] : throw std::out_of_range( "const_string access out of range" ); }
	constexpr size_type size( void ) const { return _sz; }
	constexpr size_type length( void ) const { return _sz; }
	static constexpr size_type max_size( void ) noexcept
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
	constexpr const value_type *c_str( void ) const noexcept { return _str; }

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

	constexpr int compare( size_type pos, size_type n, const const_string &s ) const
	{
		return substr( pos, n ).compare( s );
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

	const value_type *_str{ nullptr };
	size_type _sz{ 0 };
};

template <typename streamT, typename charT, typename traitsT>
inline streamT &operator<<( streamT &os, const const_string<charT, traitsT> &s )
{
	os.write( s.begin(), static_cast<std::streamsize>( s.size() ) );
	return os;
}


////////////////////////////////////////


template <typename charT, typename traitsT>
inline constexpr bool
operator==( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT, typename S>
inline constexpr bool
operator==( const const_string<charT, traitsT> &lhs, S &&rhs )
{
	return lhs.compare( const_string<charT, traitsT>( std::forward<S>( rhs ) ) ) == 0;
}

template <typename charT, typename traitsT, typename S>
inline constexpr bool
operator==( S &&lhs, const const_string<charT, traitsT> &rhs )
{
	return rhs.compare( const_string<charT, traitsT>( std::forward<S>( lhs ) ) ) == 0;
}

template <typename charT, typename traitsT>
inline constexpr bool
operator!=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) != 0;
}

template <typename charT, typename traitsT, typename S>
inline constexpr bool
operator!=( const const_string<charT, traitsT> &lhs, S &&rhs )
{
	return lhs.compare( const_string<charT, traitsT>( std::forward<S>( rhs ) ) ) != 0;
}

template <typename charT, typename traitsT, typename S>
inline constexpr bool
operator!=( S &&lhs, const const_string<charT, traitsT> &rhs )
{
	return rhs.compare( const_string<charT, traitsT>( std::forward<S>( lhs ) ) ) != 0;
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
			  ( std::is_same<Ret,RetT>::value == false &&
				( tmp < static_cast<RetT>( std::numeric_limits<Ret>::min() ) ||
				  tmp > static_cast<RetT>( std::numeric_limits<Ret>::max() ) ) ) )
		throw std::out_of_range( fname );
	else
		ret = static_cast<Ret>( tmp );
	if ( pos )
		*pos = static_cast<std::size_t>( endP - s );

	return ret;
}

} //namespace detail


inline int stoi( cstring s, std::size_t *pos = nullptr, int base = 10 ) // NOLINT
{
	return detail::stoa<long,int>( &std::strtol, "base::const_string::stoi", s.data(), pos, base ); // NOLINT
}

inline long stol( cstring s, std::size_t *pos = nullptr, int base = 10 ) // NOLINT
{
	return detail::stoa( &std::strtol, "base::const_string::stol", s.data(), pos, base );
}

inline long long stoll( cstring s, std::size_t *pos = nullptr, int base = 10 ) // NOLINT
{
	return detail::stoa( &std::strtoll, "base::const_string::stoll", s.data(), pos, base );
}

inline unsigned long stoul( cstring s, std::size_t *pos = nullptr, int base = 10 ) // NOLINT
{
	return detail::stoa( &std::strtoul, "base::const_string::stoul", s.data(), pos, base );
}

inline unsigned long long stoull( cstring s, std::size_t *pos = nullptr, int base = 10 ) // NOLINT
{
	return detail::stoa( &std::strtoull, "base::const_string::stoull", s.data(), pos, base );
}

inline float stof( cstring s, std::size_t *pos = nullptr )
{
	return detail::stoa( &std::strtof, "base::const_string::stof", s.data(), pos );
}
inline double stod( cstring s, std::size_t *pos = nullptr )
{
	return detail::stoa( &std::strtod, "base::const_string::stod", s.data(), pos );
}
inline long double stold( cstring s, std::size_t *pos = nullptr )
{
	return detail::stoa( &std::strtold, "base::const_string::stold", s.data(), pos );
}

} // namespace base




