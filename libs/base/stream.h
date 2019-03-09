// Copyright (c) 2015 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <istream>
#include <ostream>
#include <iostream>
#include <memory>
#include "streambuf.h"


////////////////////////////////////////


namespace base
{

///
/// @brief Provides a basic istream that stores a streambuf.
///
/// This allows to have a streambuf that is managed but provides differing
/// functionality with the same interface
template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class istream_bufstore : virtual public std::basic_istream<CharT, TraitsT>
{
public:
	typedef CharT char_type;
	typedef typename TraitsT::int_type int_type;
	typedef typename TraitsT::pos_type pos_type;
	typedef typename TraitsT::off_type off_type;
	typedef TraitsT traits_type;

	typedef std::basic_istream<CharT, TraitsT> base_type;
	typedef base_streambuf<CharT, TraitsT> streambuf_type;

	istream_bufstore( std::unique_ptr<streambuf_type> &&sb )
			: base_type( sb.get() ), _sbuf( std::move( sb ) )
	{
	}

	istream_bufstore( istream_bufstore &&o )
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
			:
#else
			: base_type( std::move( o ) ),
#endif
			  _sbuf( std::move( o._sbuf ) )
	{
		set_rdbuf( _sbuf.get() );
	}

	istream_bufstore &operator=( istream_bufstore &&rhs )
	{
		base_type::operator=( std::move( rhs ) );
		_sbuf = std::move( rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		return *this;
	}
	void
	swap( istream_bufstore &rhs )
	{
		base_type::swap( rhs );
		std::swap( _sbuf, rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		rhs.set_rdbuf( rhs._sbuf.get() );
	}

	virtual ~istream_bufstore( void )
	{
	}

	//////////////////
	// members

	// Read the bit in ios_base about rdbuf and the hiding of the
	// base rdbuf function. Given that we're using a pointer,
	// seems like we should do the same thing to discourage fiddling
	streambuf_type *rdbuf( void ) const { return _sbuf.get(); }
	const std::string &uri( void ) const { return _sbuf->get_uri(); }

protected:
	inline void set_rdbuf( streambuf_type *p )
	{
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
		base_type::rdbuf( p );
#else
		base_type::set_rdbuf( p );
#endif
	}
	istream_bufstore( const istream_bufstore & ) = delete;
	istream_bufstore &operator=( const istream_bufstore & ) = delete;

private:
	std::unique_ptr<streambuf_type> _sbuf;
};


///
/// @brief Provides a basic ostream that stores a streambuf by pointer.
///
/// This allows to have a streambuf that is managed but provides differing
/// functionality with the same interface
template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class ostream_bufstore : virtual public std::basic_ostream<CharT, TraitsT>
{
public:
	typedef CharT char_type;
	typedef typename TraitsT::int_type int_type;
	typedef typename TraitsT::pos_type pos_type;
	typedef typename TraitsT::off_type off_type;
	typedef TraitsT traits_type;

	typedef std::basic_ostream<CharT, TraitsT> base_type;
	typedef base_streambuf<CharT, TraitsT> streambuf_type;

	ostream_bufstore( std::unique_ptr<streambuf_type> &&sb )
			: base_type(), _sbuf( std::move( sb ) )
	{
		this->init( _sbuf.get() );
	}

	ostream_bufstore( ostream_bufstore &&o )
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
			:
#else
			: base_type( std::move( o ) ),
#endif
			  _sbuf( std::move( o._sbuf ) )
	{
		set_rdbuf( _sbuf.get() );
	}

	ostream_bufstore &operator=( ostream_bufstore &&rhs )
	{
		base_type::operator=( std::move( rhs ) );
		_sbuf = std::move( rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		return *this;
	}

	void swap( ostream_bufstore &rhs )
	{
		base_type::swap( rhs );
		std::swap( _sbuf, rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		rhs.set_rdbuf( rhs._sbuf.get() );
	}

	virtual ~ostream_bufstore( void )
	{
	}

	//////////////////
	// members

	// Read the bit in ios_base about rdbuf and the hiding of the
	// base rdbuf function. Given that we're using a pointer,
	// seems like we should do the same thing to discourage fiddling
	streambuf_type *rdbuf( void ) const { return _sbuf.get(); }
	const std::string &uri( void ) const { return _sbuf->get_uri(); }

protected:
	inline void set_rdbuf( streambuf_type *p )
	{
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
		base_type::rdbuf( p );
#else
		base_type::set_rdbuf( p );
#endif
	}

	ostream_bufstore( const ostream_bufstore & ) = delete;
	ostream_bufstore &operator=( const ostream_bufstore & ) = delete;

private:
	std::unique_ptr<streambuf_type> _sbuf;
};


///
/// @brief Provides a basic ostream that stores a streambuf by pointer.
///
/// This allows to have a streambuf that is managed but provides differing
/// functionality with the same interface
template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class iostream_bufstore : virtual public std::basic_iostream<CharT, TraitsT>
{
public:
	typedef CharT char_type;
	typedef typename TraitsT::int_type int_type;
	typedef typename TraitsT::pos_type pos_type;
	typedef typename TraitsT::off_type off_type;
	typedef TraitsT traits_type;

	typedef std::basic_iostream<CharT, TraitsT> base_type;
	typedef base_streambuf<CharT, TraitsT> streambuf_type;

	iostream_bufstore( std::unique_ptr<streambuf_type> &&sb )
			: base_type( sb.get() ), _sbuf( std::move( sb ) )
	{
	}

	iostream_bufstore( iostream_bufstore &&o )
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
			:
#else
			: base_type( std::move( o ) ),
#endif
			  _sbuf( std::move( o._sbuf ) )
	{
		set_rdbuf( _sbuf.get() );
	}

	iostream_bufstore &operator=( iostream_bufstore &&rhs )
	{
		base_type::operator=( std::move( rhs ) );
		_sbuf = std::move( rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		return *this;
	}

	void swap( iostream_bufstore &rhs )
	{
		base_type::swap( rhs );
		std::swap( _sbuf, rhs._sbuf );
		set_rdbuf( _sbuf.get() );
		rhs.set_rdbuf( rhs._sbuf.get() );
	}

	virtual ~iostream_bufstore( void )
	{
	}

	//////////////////
	// members

	// Read the bit in ios_base about rdbuf and the hiding of the
	// base rdbuf function. Given that we're using a pointer,
	// seems like we should do the same thing to discourage fiddling
	streambuf_type *rdbuf( void ) const { return _sbuf.get(); }
	const std::string &uri( void ) const { return _sbuf->get_uri(); }

protected:
	inline void set_rdbuf( streambuf_type *p )
	{
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
		base_type::rdbuf( p );
#else
		base_type::set_rdbuf( p );
#endif
	}

	iostream_bufstore( const iostream_bufstore & ) = delete;
	iostream_bufstore &operator=( const iostream_bufstore & ) = delete;

private:
	std::unique_ptr<streambuf_type> _sbuf;
};

typedef istream_bufstore<char> istream;
typedef ostream_bufstore<char> ostream;
typedef iostream_bufstore<char> iostream;

typedef istream_bufstore<wchar_t> wistream;
typedef ostream_bufstore<wchar_t> wostream;
typedef iostream_bufstore<wchar_t> wiostream;

} // namespace base



