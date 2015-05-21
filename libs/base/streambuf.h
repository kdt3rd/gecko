
#pragma once

#include "pointer.h"
#include <streambuf>

namespace base
{

////////////////////////////////////////

/// @brief Stream buffer class
///
/// Simplify writing stream buffer classes by abstracting away buffering and other complex behaviors of std::streambuf.
template<class Char = char, class Traits = std::char_traits<Char>>
class streambuf : public std::basic_streambuf<Char,Traits>
{
public:
	streambuf( void )
	{
		_gbuffer = std::shared_ptr<Char>( new Char[_gsize+1], array_deleter<Char>() );
		_pbuffer = std::shared_ptr<Char>( new Char[_psize+1], array_deleter<Char>() );
	}

protected:
	typedef typename Traits::pos_type pos_type;
	typedef typename Traits::int_type int_type;
	typedef typename Traits::off_type off_type;

	/// @brief Abstract write to be implemented by subclass.
	virtual std::streamsize write( const void *buf, std::streamsize bytes ) = 0;

	/// @brief Abstract read to be implemented by subclass.
	virtual std::streamsize read( void *buf, std::streamsize bytes ) = 0;

	/// @brief Sets the position indicator of the input and/or output sequence to an absolute position.
	pos_type seekpos( pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out ) override
	{
		return seekoff( pos, std::ios_base::beg, which );
	}

	/// @brief Synchronizes the controlled character sequence (the buffers) with the associated character sequence.
	int sync( void ) override
	{
		// sync put buffers
		Char *buf = _pbuffer.get();
		std::streamsize n = this->pptr() - this->pbase();
		if ( n > 0 )
			write( buf, n );
		setp( buf, buf + _psize );

		// sync get buffers
		buf = _gbuffer.get();
		setg( buf, buf + _gsize, buf + _gsize );

		return 0;
	}

	/// @brief Ensures that at least one character is available in the input area.
	int_type underflow( void ) override
	{
		if ( this->gptr() == this->egptr() )
		{
			char *buf = _gbuffer.get();
			std::streamsize n = read( buf, std::max( std::min( showmanyc(), _gsize ), std::streamsize( 1 ) ) );
			this->setg( buf, buf, buf + n );
			if ( n == 0 ) return Traits::eof();
		}
		return Traits::to_int_type( *(this->gptr()) );
	}

	/// @brief Ensures that there is space at the put area for at least one character.
	int_type overflow( int_type ch = Traits::eof() ) override
	{
		if ( Traits::eq_int_type( ch, Traits::eof() ) )
			return sync();

		if ( this->pptr() == this->epptr() )
			sync();

		*(this->pptr()) = (Char)ch;
		this->pbump( 1 );

		return ch;
	}

	/// @brief Writes count characters to the output sequence from a character array
	std::streamsize xsputn( const Char *s, std::streamsize count ) override
	{
		if ( _psize == 0 )
			// Write directly if there's no buffer
			return write( s, count );
		else
			// Otherwise let the base class buffer it and call overflow as needed.
			return std::streambuf::xsputn( s, count );
	}


	/// @brief Reads count characters from the input sequence and stores them into a character array.
	std::streamsize xsgetn( Char *s, std::streamsize count ) override
	{
		std::streamsize ret = 0;

		while ( count > 0 )
		{
			std::streamsize bufd = this->egptr() - this->gptr();
			std::streamsize len = 0;
			if ( bufd > 0 )
			{
				len = std::min( bufd, count );
				Traits::copy( s, this->gptr(), len );
				this->gbump( static_cast<int>( len ) );
			}
			else
			{
				if ( count > _gsize )
				{
					len = read( s, count );
					if ( len == 0 )
						break;
				}
				else
				{
					if ( Traits::eq_int_type( underflow(), Traits::eof() ) )
						break;
				}
			}
			s += len;
			count -= len;
			ret += len;
		}

		return ret;
	}


	/// @brief Estimates the number of characters available for input.
	std::streamsize showmanyc( void ) override
	{
		return _gsize;
	}

private:
	std::streamsize _gsize = 0;
	std::streamsize _psize = 0;
	std::shared_ptr<Char> _gbuffer;
	std::shared_ptr<Char> _pbuffer;
};

////////////////////////////////////////

}

