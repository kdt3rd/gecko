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

inline size_t const_length( const char *s ) { return std::strlen( s ); }
inline size_t const_length( const wchar_t *s ) { return std::wcslen( s ); }

	
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
	constexpr const_string( const value_type *s, size_type N ) : _str( s ), _sz( N - 1 ) {}
	template <typename allocTs>
	constexpr const_string( const std::basic_string<charT, traitsT, allocTs> &s ) : _str( s.data() ), _sz( s.size() ) {}
	constexpr const_string( const value_type *s ) : _str( s ), _sz( const_length( s ) ) {}

	constexpr const_string( const const_string & ) noexcept = default;
	const_string &operator=( const const_string & ) noexcept = default;

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

	template <typename allocT>
	int
	compare( const std::basic_string<value_type, traits_type, allocT> &s ) const
	{
		int rval = traits_type::compare( data(), s.data(), std::min( s.size(), size() ) );
		if ( rval != 0 )
			return rval;

		return ( size() < s.size() ) ? -1 : ( ( size() > s.size() ) ? 1 : 0 );
	}

	constexpr int
	compare( const const_string &s ) const
	{
		return ( size() <= s.size() ?
				 compare_priv( begin(), s.begin(), size(), size() == s.size() ? 0 : -1 ) :
				 compare_priv( begin(), s.begin(), s.size(), 1 ) );
	}

	template <std::size_t N>
	constexpr int
	compare( const value_type (&s)[N] ) const
	{
		return ( size() <= (N-1) ?
				 compare_priv( begin(), s, size(), size() == (N-1) ? 0 : -1 ) :
				 compare_priv( begin(), s, (N-1), 1 ) );
	}

	constexpr int compare( size_type pos, size_type n, const const_string &s ) const
	{
		return substr( pos, n ).compare( s );
	}

	template <std::size_t N>
	constexpr int compare( size_type pos, size_type n, const value_type (&s)[N] ) const
	{
		return substr( pos, n ).compare( const_string( s, N ) );
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

	constexpr size_type find_first_of( value_type c, size_type pos = 0 ) const
	{
		return ( pos < size() ?
				 ( traits_type::eq( _str[pos], c ) ? pos : find_first_of( c, pos + 1 ) ) :
				 npos );
	}

	constexpr size_type find_first_not_of( value_type c, size_type pos = 0 ) const
	{
		return ( pos < size() ?
				 ( ! traits_type::eq( _str[pos], c ) ? pos : find_first_of( c, pos + 1 ) ) :
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

template <typename charT, typename traitsT>
inline bool
operator==( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT, std::size_t N>
inline bool
operator==( const const_string<charT, traitsT> &lhs, const charT (&rhs)[N] )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT, std::size_t N>
inline bool
operator==( const charT (&lhs)[N], const const_string<charT, traitsT> &rhs )
{
	return rhs.compare( lhs ) == 0;
}

template <typename charT, typename traitsT, typename allocT>
inline bool
operator==( const std::basic_string<charT, traitsT, allocT> &lhs, const const_string<charT, traitsT> &rhs )
{
	if ( lhs.size() == rhs.size() )
		return rhs.compare( lhs ) == 0;
	return false;
}

template <typename charT, typename traitsT, typename allocT>
inline bool
operator==( const const_string<charT, traitsT> &lhs, const std::basic_string<charT, traitsT, allocT> &rhs )
{
	if ( lhs.size() == rhs.size() )
		return lhs.compare( rhs ) == 0;
	return false;
}

template <typename charT, typename traitsT>
inline bool
operator!=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT, std::size_t N>
inline bool
operator!=( const const_string<charT, traitsT> &lhs, const charT (&rhs)[N] )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT, std::size_t N>
inline bool
operator!=( const charT (&lhs)[N], const const_string<charT, traitsT> &rhs )
{
	return !( rhs == lhs );
}

template <typename charT, typename traitsT, typename allocT>
inline bool
operator!=( const std::basic_string<charT, traitsT, allocT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT, typename allocT>
inline bool
operator!=( const const_string<charT, traitsT> &lhs, const std::basic_string<charT, traitsT, allocT> &rhs )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT>
inline bool
operator<( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) < 0;
}

template <typename charT, typename traitsT>
inline bool
operator>( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) > 0;
}

template <typename charT, typename traitsT>
inline bool
operator<=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) <= 0;
}

template <typename charT, typename traitsT>
inline bool
operator>=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) >= 0;
}

} // namespace base




