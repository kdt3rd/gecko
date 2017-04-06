//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "compiler_abi.h"
#include <memory>
#include <streambuf>
#ifdef HAS_BAD_CODECVT_HEADER
# include <bits/codecvt.h>
#else
# include <codecvt>
#endif
#include <cstring>
#include <string>
#include <stdexcept>
#include "utility.h"
#include "contract.h"


////////////////////////////////////////


namespace base
{

/// @brief Provides a base class for a managed rdbuf
///        of arbitrary capability and type.
///
/// A subclass of this can be a network socket, a file, whatever.
template <class CharT, class TraitsT = std::char_traits<CharT>>
class base_streambuf : public std::basic_streambuf<CharT, TraitsT>
{
protected:
	typedef CharT char_type;
	typedef typename TraitsT::int_type int_type;
	typedef typename TraitsT::pos_type pos_type;
	typedef typename TraitsT::off_type off_type;
	typedef TraitsT traits_type;

	typedef typename std::basic_streambuf<CharT, TraitsT> base_type;
	typedef typename traits_type::state_type codecvt_state_type;
	typedef std::codecvt<char_type, char, codecvt_state_type> codecvt_type;

	template <typename F> inline const F &check_facet( const F *f )
	{
		if ( ! f )
			throw std::bad_cast();
		return *f;
	}

public:
	base_streambuf( std::ios_base::openmode mode, std::streamsize bufSz = 0 )
			: _open_mode( mode ), _buf_sz( bufSz )
	{
		_buf_sz = std::max( std::streamsize(1), _buf_sz );
		_buf_store.reset( new char_type[static_cast<size_t>( _buf_sz )] );
		_buf = _buf_store.get();
		update_base_buffer_pointers( -1 );

		if ( std::has_facet<codecvt_type>( this->getloc() ) )
			_codecvt_cache = &std::use_facet<codecvt_type>( this->getloc() );
	}
	virtual ~base_streambuf( void )
	{
		this->close();
	}

	void stash_uri( const std::string &u ) { _uri = u; }
	void stash_uri( std::string &&u ) { _uri = std::move( u ); }
	const std::string &get_uri( void ) const { return _uri; }

protected:
	base_streambuf( const base_streambuf & ) = delete;
	base_streambuf &operator=( const base_streambuf & ) = delete;

	/// @brief Move constructor
	///
	/// Sub-classes should provide this if they aren't going to be
	/// used in a shared_ptr
	base_streambuf( base_streambuf &&b )
#ifdef HAS_MISSING_STREAM_MOVE_CTORS
			:
#else
			: base_type( b ),
#endif
			  _open_mode( base::exchange( b._open_mode, std::ios_base::openmode(0) ) ),
			  _buf_sz( base::exchange( b._buf_sz, 1 ) ),
			  _writing( base::exchange( b._writing, false ) ),
			  _reading( base::exchange( b._reading, false ) ),
			  _buf( base::exchange( b._buf, nullptr ) ),
			  _buf_store( std::move( b._buf_store ) ),
			  _state_beg( std::move( b._state_beg ) ),
			  _state_cur( std::move( b._state_cur ) ),
			  _state_last( std::move( b._state_last ) ),
			  _codecvt_cache( b._codecvt_cache ),
			  _conv_buf_store( std::move( b._conv_buf_store ) ),
			  _conv_buf_sz( base::exchange( b._conv_buf_sz, 0 ) ),
			  _conv_buf( base::exchange( b._conv_buf, nullptr ) ),
			  _conv_next( base::exchange( b._conv_next, nullptr ) ),
			  _conv_end( base::exchange( b._conv_end, nullptr ) ),
			  _pback( b._pback ),
			  _pback_save_cur( base::exchange( b._pback_save_cur, nullptr ) ),
			  _pback_save_end( base::exchange( b._pback_save_end, nullptr ) ),
			  _pback_live( base::exchange( b._pback_live, false ) )
	{
		b.update_base_buffer_pointers( -1 );
		b._state_last = b._state_cur = b._state_beg;
	}

	/// Sub-classes should provide this if they aren't going to be
	/// used in a shared_ptr
	base_streambuf &operator=( base_streambuf &&b )
	{
		this->close();
#ifndef HAS_MISSING_STREAM_MOVE_CTORS
		base_type::operator=( b );
#endif
		_open_mode = base::exchange( b._open_mode, std::ios_base::openmode(0) );
		_buf_sz = base::exchange( b._buf_sz, 1 );
		_writing = base::exchange( b._writing, false );
		_reading = base::exchange( b._reading, false );
		_buf = base::exchange( b._buf, nullptr );
		_buf_store = std::move( b._buf_store );
		_state_beg = std::move( b._state_beg );
		_state_cur = std::move( b._state_cur );
		_state_last = std::move( b._state_last );
		_codecvt_cache = b._codecvt_cache;
		_conv_buf_store = std::move( b._conv_buf_store );
		_conv_buf_sz = base::exchange( b._conv_buf_sz, 0 );
		_conv_buf = base::exchange( b._conv_buf, nullptr );
		_conv_next = base::exchange( b._conv_next, nullptr );
		_conv_end = base::exchange( b._conv_end, nullptr );
		_pback = b._pback;
		_pback_save_cur = base::exchange( b._pback_save_cur, nullptr );
		_pback_save_end = base::exchange( b._pback_save_end, nullptr );
		_pback_live = base::exchange( b._pback_live, false );
		b.update_base_buffer_pointers( -1 );
		b._state_last = b._state_cur = b._state_beg;
		return *this;
	}

	/// Sub-classes should provide this if appropriate
	void swap( base_streambuf &b )
	{
#ifndef HAS_MISSING_STREAM_MOVE_CTORS
		base_type::swap( b );
#endif
		std::swap( _open_mode, b._open_mode );
		std::swap( _buf_sz, b._buf_sz );
		std::swap( _reading, b._reading );
		std::swap( _writing, b._writing );
		std::swap( _buf, b._buf );
		std::swap( _buf_store, b._buf_store );
		std::swap( _state_beg, b._state_beg );
		std::swap( _state_cur, b._state_cur );
		std::swap( _state_last, b._state_last );
		std::swap( _codecvt_cache, b._codecvt_cache );
		std::swap( _conv_buf_store, b._conv_buf_store );
		std::swap( _conv_buf_sz, b._conv_buf_sz );
		std::swap( _conv_buf, b._conv_buf );
		std::swap( _conv_next, b._conv_next );
		std::swap( _conv_end, b._conv_end );
		std::swap( _pback, b._pback );
		std::swap( _pback_save_cur, b._pback_save_cur );
		std::swap( _pback_save_end, b._pback_save_end );
		std::swap( _pback_live, b._pback_live );
	}

protected:
	/// @name Virtual methods
	/// Subclasses should consider implementing these functions
	/// @{

	/// @brief Returns whether the streambuf is in an open state and
	/// can be read / written to
	virtual bool is_open( void ) const
	{
		return true;
	}

	/// @brief Should close the underlying store if applicable
	virtual void close( void ) noexcept
	{
	}

	/// @brief Should return the number of bytes available
	///
	/// Files and strings should return the size - current position,
	/// things like network sockets can do ioctl( FIONREAD )
	/// and others that are unknown can use the default return of -1
	virtual off_type bytes_avail( void )
	{
		return off_type(-1);
	}

	/// @brief Perform a seek on the underlying store
	virtual off_type seek( off_type /*off*/, std::ios_base::seekdir /*dir*/ )
	{
		return off_type(-1);
	}

	/// @brief Perform a read from the underlying store
	virtual std::streamsize read( void * /*outBuf*/, size_t /*numBytes*/ )
	{
		return ssize_t(-1);
	}

	/// @brief Perform a write to the underlying store
	virtual std::streamsize write( const void * /*outBuf*/, size_t /*numBytes*/ )
	{
		return ssize_t(-1);
	}

	virtual std::streamsize writev( const void *outBuf1, size_t numBytes1,
									const void *outBuf2, size_t numBytes2 )
	{
		std::streamsize ret = 0;
		if ( numBytes1 )
			ret = this->write( outBuf1, numBytes1 );

		if ( ret == static_cast<std::streamsize>(numBytes1) )
			ret += this->write( outBuf2, numBytes2 );

		return ret;
	}

	/// @}

	/// @name Helper functions
	/// Subclassed functions from streambuf that are implemented by default
	/// such that subclasses of this are much simpler
	/// @{
	///
protected:
	////// locale handling
	virtual void imbue( const std::locale &l ) override
	{
		base_type::imbue( l );
		const codecvt_type *tmp = nullptr;
		if ( std::has_facet<codecvt_type>( l ) )
			tmp = &std::use_facet<codecvt_type>( l );

		bool valid = true;
		if ( this->is_open() )
		{
			// ok only at the beginning if changing encoding
			if ( ( _reading || _writing ) &&
				 check_facet( _codecvt_cache ).encoding() == -1 )
			{
				valid = false;
			}
			else
			{
				if ( _reading )
				{
					if ( check_facet( _codecvt_cache ).always_noconv()  )
					{
						if ( tmp && ! check_facet( tmp ).always_noconv() )
						{
							// if we can seek, it's ok
							valid = ( this->seekoff( 0, std::ios_base::cur, _open_mode ) != pos_type( off_type( -1 ) ) );
						}
					}
					else
					{
						_conv_next = _conv_buf +
							_codecvt_cache->length( _state_last, _conv_buf,
													_conv_next,
													static_cast<size_t>( this->gptr() - this->eback() ) );
						const std::streamsize rem = _conv_end - _conv_next;
						if ( rem > 0 )
							std::memmove( _conv_buf, _conv_next, static_cast<size_t>( rem ) );

						_conv_next = _conv_buf;
						_conv_end = _conv_buf + rem;
						update_base_buffer_pointers( -1 );
						_state_last = _state_cur = _state_beg;
					}
				}
				else if ( _writing )
				{
					valid = terminate_output();
					if ( valid )
						update_base_buffer_pointers( -1 );
				}
			}
		}

		if ( valid )
			_codecvt_cache = tmp;
		else
			_codecvt_cache = nullptr;
	}

	////// positioning
	virtual std::streambuf *setbuf( char_type *s, std::streamsize n ) override
	{
		// if we're already open, who knows what state we're in, don't
		// allow the buffer to change
		if ( this->is_open() )
			return this;

		if ( ( n > _buf_sz ) || s )
			_buf_store.reset();

		_buf_sz = std::max( std::streamsize(0), n );
		_buf = s;
		if ( ! _buf )
		{
			if ( _buf_sz == 0 )
				_buf_sz = 1;
			_buf_store.reset( new char_type[static_cast<size_t>(_buf_sz)] );
			_buf = _buf_store.get();
		}
		update_base_buffer_pointers( -1 );

		return this;
	}

	virtual pos_type seekoff( off_type off, std::ios_base::seekdir dir,
							  std::ios_base::openmode /*which*/ = std::ios_base::in | std::ios_base::out ) override
	{
		int width = 0;
		if ( _codecvt_cache )
			width = _codecvt_cache->encoding();
		if ( width < 0 )
			width = 0;

		pos_type ret = pos_type( off_type(-1) );
		const bool failed = ( off != 0 && width <= 0 );
		if ( is_open() && ! failed )
		{
			bool no_movement = dir == std::ios_base::cur && off == 0 &&
				( ! _writing || _codecvt_cache->always_noconv() );

			// avoid problems with pback buffers
			if ( ! no_movement )
				clear_pback();
			codecvt_state_type state = _state_beg;
			off_type computed_off = off * width;
			if ( _reading && dir == std::ios_base::cur )
			{
				state = _state_last;
				computed_off += get_ext_pos( state );
			}

			if ( ! no_movement )
			{
				ret = seek_priv( computed_off, dir, state );
			}
			else
			{
				if ( _writing )
					computed_off = this->pptr() - this->pbase();

				off_type file_off = this->seek( 0, std::ios_base::cur );
				if ( file_off != off_type(-1) )
				{
					ret = file_off + computed_off;
					ret.state( state );
				}
			}
		}
		return ret;
	}

	virtual pos_type seekpos( pos_type pos, std::ios_base::openmode /*which*/ = std::ios_base::in | std::ios_base::out ) override
	{
		pos_type ret = pos_type( off_type(-1) );
		if ( this->is_open() )
		{
			clear_pback();
			ret = seek_priv( off_type( pos ), std::ios_base::beg, pos.state() );
		}
		return ret;
	}

	virtual int sync( void ) override
	{
		if ( this->pbase() < this->pptr() )
		{
			const int_type t = this->overflow();
			if ( traits_type::eq_int_type( t, traits_type::eof() ) )
				return -1;
		}
		return 0;
	}

	////// read functions
	virtual std::streamsize showmanyc( void ) override
	{
		std::streamsize ret = -1;
		if ( ( _open_mode & std::ios_base::in ) && this->is_open() )
		{
			ret = this->egptr() - this->gptr();

			if ( check_facet( _codecvt_cache ).encoding() >= 0 )
				ret += this->bytes_avail() / _codecvt_cache->max_length();
		}

		return ret;
	}

	virtual int_type underflow( void ) override
	{
		int_type ret = traits_type::eof();
		if ( _open_mode & std::ios_base::in )
		{
			if ( _writing )
			{
				if ( this->overflow() == traits_type::eof() )
					return ret;
				update_base_buffer_pointers( -1 );
				_writing = false;
			}

			clear_pback();
			// if pback restored a buffer, just return that now prior
			// to an actual read
			if ( this->gptr() < this->egptr() )
				return traits_type::to_int_type( *this->gptr() );

			const std::streamsize buflen = _buf_sz > 1 ? _buf_sz - 1 : 1;
			bool gotEOF = false;
			std::streamsize ilen = 0;
			std::codecvt_base::result r = std::codecvt_base::ok;
			if ( check_facet( _codecvt_cache ).always_noconv() )
			{
				ilen = read( this->eback(), static_cast<size_t>( buflen ) * sizeof(char_type) );
				gotEOF = ( ilen == 0 );
			}
			else
			{
				const int enc = _codecvt_cache->encoding();
				std::streamsize blen;
				std::streamsize rlen;
				if ( enc > 0 )
					blen = rlen = buflen * enc;
				else
				{
					blen = buflen + _codecvt_cache->max_length() - 1;
					rlen = buflen;
				}
				const std::streamsize rem = _conv_end - _conv_next;
				rlen = rlen > rem ? rlen - rem : 0;

				if ( _reading && this->egptr() == this->eback() && rem )
					rlen = 0;

				if ( _conv_buf_sz < blen )
				{
					std::unique_ptr<char[]> buf( new char[static_cast<size_t>(blen)] );
					if ( rem )
						std::memcpy( buf.get(), _conv_next, static_cast<size_t>( rem ) );
					_conv_buf_store = std::move( buf );
					_conv_buf = _conv_buf_store.get();
					_conv_buf_sz = blen;
				}
				else if ( rem )
					std::memmove( _conv_buf, _conv_next, static_cast<size_t>( rem ) );

				_conv_next = _conv_buf;
				_conv_end = _conv_buf + rem;
				_state_last = _state_cur;
				do
				{
					if ( rlen > 0 )
					{
						precondition( ( _conv_end - _conv_buf + rlen ) <= _conv_buf_sz,
									  "codecvt::max_length() is not valid" );
						std::streamsize elen = read( _conv_end, static_cast<size_t>( rlen ) );
						if ( elen == 0 )
							gotEOF = true;
						else if ( elen == -1 )
							break;
						_conv_end += elen;
					}

					char_type *iend = this->eback();
					if ( _conv_next < _conv_end )
						r = _codecvt_cache->in( _state_cur, _conv_next,
												_conv_end, _conv_next,
												this->eback(),
												this->eback() + buflen, iend );
					if ( r == std::codecvt_base::noconv )
					{
						std::streamsize avail = _conv_end - _conv_buf;
						ilen = std::min( avail, buflen );
						traits_type::copy( this->eback(),
										   reinterpret_cast<char_type*>( _conv_buf ),
										   static_cast<size_t>( ilen ) );
						_conv_next = _conv_buf + ilen;
					}
					else
						ilen = iend - this->eback();

					if ( r == std::codecvt_base::error )
						break;
					rlen = 1;
				} while ( ilen == 0 && ! gotEOF );
			}

			if ( ilen > 0 )
			{
				update_base_buffer_pointers( ilen );
				_reading = true;
				ret = traits_type::to_int_type( *this->gptr() );
			}
			else if ( gotEOF )
			{
				// we're at EOF, switch to not reading / uncommited mode
				// so we can write without seeking
				update_base_buffer_pointers( -1 );
				_reading = false;
				// although if we have a partial char, that's an error
				if ( r == std::codecvt_base::partial )
					throw std::runtime_error( "Incomplete character code in stream" );
			}
			else if ( r == std::codecvt_base::error )
				throw std::runtime_error( "Invalid byte sequence in stream" );
			else
				throw std::runtime_error( "Error reading stream" );
		}

		return ret;
	}

	// no need to re-implement uflow as that is the same as underflow for
	// what we care about

	virtual std::streamsize xsgetn( char_type *s, std::streamsize count ) override
	{
		std::streamsize ret = 0;
		if ( _pback_live )
		{
			if ( count > 0 && this->gptr() == this->eback() )
			{
				*s++ = *this->gptr();
				this->gbump( 1 );
				ret = 1;
				--count;
			}
			clear_pback();
		}
		else if ( _writing )
		{
			if ( overflow() == traits_type::eof() )
				return ret;
			update_base_buffer_pointers( -1 );
			_writing = false;
		}

		const std::streamsize buflen = _buf_sz > 1 ? _buf_sz - 1 : 1;

		// optimization to avoid double copy - just read straight through
		if ( count > buflen && check_facet( _codecvt_cache ).always_noconv() &&
			 ( _open_mode & std::ios_base::in ) )
		{
			// any chars already in the buffer?
			const std::streamsize avail = this->egptr() - this->gptr();
			if ( avail > 0 )
			{
				traits_type::copy( s, this->gptr(), static_cast<size_t>( avail ) );
				s += avail;
				this->setg( this->eback(), this->gptr() + avail, this->egptr() );
				ret += avail;
				count -= avail;
			}

			std::streamsize nread;
			// loop in case of short read from socket or pipe
			do
			{
				nread = this->read( s, static_cast<size_t>(count) * sizeof(char_type) );
				if ( nread == -1 )
					throw std::runtime_error( "Error reading from streambuf" );
				if ( nread == 0 )
					break;

				count -= nread;
				ret += nread;
				if ( count == 0 )
					break;
				s += nread;
			}
			while ( true );

			if ( count == 0 )
			{
				update_base_buffer_pointers( 0 );
				_reading = true;
			}
			else if ( nread == 0 )
			{
				update_base_buffer_pointers( -1 );
				_reading = false;
			}
		}
		else
			ret += base_type::xsgetn( s, count );

		return ret;
	}

	////// write functions
	virtual std::streamsize xsputn( const char_type *s, std::streamsize count ) override
	{
		std::streamsize ret = 0;
		// optimize larger writes straight through when no conversion is necessary
		if ( check_facet( _codecvt_cache ).always_noconv() &&
			 ( _open_mode & std::ios_base::out || _open_mode & std::ios_base::app ) &&
			 ! _reading )
		{
			const std::streamsize chunkSize = 4096;
			std::streamsize bufavail = this->epptr() - this->pptr();

			if ( ! _writing && _buf_sz > 1 )
				bufavail = _buf_sz - 1;

			const std::streamsize lim = std::min( chunkSize, bufavail );
			if ( count >= lim )
			{
				const std::streamsize charSize = sizeof(char_type);
				const std::streamsize bufCount = (this->pptr() - this->pbase()) * charSize;
				const std::streamsize countOut = count * charSize;
				ret = this->writev( this->pbase(), static_cast<size_t>(bufCount), s, static_cast<size_t>(countOut) );

				if ( ret == (bufCount + countOut) )
				{
					update_base_buffer_pointers( 0 );
					_writing = true;
				}

				if ( ret > bufCount )
					ret -= bufCount/charSize;
				else
					ret = 0;
			}
			else
				ret = base_type::xsputn( s, count );
		}
		else
			ret = base_type::xsputn( s, count );

		return ret;
	}

	virtual int_type overflow( int_type ch = traits_type::eof() ) override
	{
		int_type ret = traits_type::eof();
		const bool isEOF = traits_type::eq_int_type( ch, ret );

		if ( _open_mode & std::ios_base::out ||
			 _open_mode & std::ios_base::app )
		{
			if ( _reading )
			{
				clear_pback();
				const off_type gptr_off = get_ext_pos( _state_last );
				if ( seek_priv( gptr_off, std::ios_base::cur, _state_last ) == pos_type( off_type(-1) ) )
					return ret;
			}

			if ( this->pbase() < this->pptr() )
			{
				if ( ! isEOF )
				{
					*this->pptr() = traits_type::to_char_type( ch );
					this->pbump( 1 );
				}

				if ( convertExternal( this->pbase(), this->pptr() - this->pbase() ) )
				{
					update_base_buffer_pointers( 0 );
					ret = traits_type::not_eof( ch );
				}
			}
			else if ( _buf_sz > 1 )
			{
				update_base_buffer_pointers( 0 );
				_writing = true;
				if ( ! isEOF )
				{
					*this->pptr() = traits_type::to_char_type( ch );
					this->pbump( 1 );
				}
				ret = traits_type::not_eof( ch );
			}
			else
			{
				// totally unbuffered
				char_type conv = traits_type::to_char_type( ch );
				if ( isEOF || convertExternal( &conv, 1 ) )
				{
					_writing = true;
					ret = traits_type::not_eof( ch );
				}
			}
		}

		return ret;
	}

	////// putback
	virtual int_type pbackfail( int_type c = traits_type::eof() ) override
	{
		int_type ret = traits_type::eof();

		if ( _open_mode & std::ios_base::in )
		{
			if ( _writing )
			{
				if ( overflow() == traits_type::eof() )
					return ret;
				update_base_buffer_pointers( -1 );
				_writing = false;
			}

			const bool isPB = _pback_live;
			const bool isEOF = traits_type::eq_int_type( c, ret );
			int_type tmpC;
			if ( this->eback() < this->gptr() )
			{
				this->gbump( -1 );
				tmpC = traits_type::to_int_type( *this->gptr() );
			}
			else if ( this->seekoff( -1, std::ios_base::cur ) != pos_type(off_type(-1)) )
			{
				tmpC = this->underflow();
				if ( traits_type::eq_int_type( tmpC, ret ) )
					return ret;
			}
			else
			{
				// at the beginning or the seek failed, return eof indicating
				// we couldn't pback
				return ret;
			}

			if ( ! isEOF && traits_type::eq_int_type( c, tmpC ) )
				ret = c;
			else if ( isEOF )
				ret = traits_type::not_eof( c );
			else if ( ! isPB )
			{
				fill_pback();
				_reading = true;
				*this->gptr() = traits_type::to_char_type( c );
				ret = c;
			}
		}

		return ret;
	}

	/// @}
	
protected:
	inline void update_base_buffer_pointers( std::streamsize off )
	{
		const bool isIn = _open_mode & std::ios_base::in;
		const bool isOut = ( _open_mode & std::ios_base::out ||
							 _open_mode & std::ios_base::app );

		if ( isIn && off > 0 )
			this->setg( _buf, _buf, _buf + off );
		else
			this->setg( _buf, _buf, _buf );

		if ( isOut && off == 0 && _buf_sz > 1 )
			this->setp( _buf, _buf + _buf_sz - 1 );
		else
			this->setp( nullptr, nullptr );
	}

	inline void fill_pback( void )
	{
		if ( ! _pback_live )
		{
			_pback_save_cur = this->gptr();
			_pback_save_end = this->egptr();
			this->setg( &_pback, &_pback, &_pback + 1 );
			_pback_live = true;
		}
	}

	inline void clear_pback( void ) noexcept
	{
		if ( _pback_live )
		{
			_pback_save_cur += this->gptr() != this->eback();
			this->setg( _buf, _pback_save_cur, _pback_save_end );
			_pback_live = false;
		}
	}

	inline bool convertExternal( char_type *ibuf, std::streamsize ilen )
	{
		std::streamsize elen, plen;

		if ( check_facet( _codecvt_cache ).always_noconv() )
		{
			elen = write( ibuf, static_cast<size_t>(ilen) * sizeof(char_type) );
			if ( elen >= 0 )
				elen /= sizeof(char_type);
			plen = ilen;
		}
		else
		{
			std::streamsize blen = ilen * _codecvt_cache->max_length();
			char locbuf[16];
			char *buf = locbuf;

			std::unique_ptr<char[]> locbufa;
			if ( blen > 16 )
			{
				locbufa.reset( new char[static_cast<size_t>(blen)] );
				buf = locbufa.get();
			}

			char *bend;
			const char_type *iend;
			auto r = _codecvt_cache->out( _state_cur, ibuf, ibuf + ilen,
										  iend, buf, buf + blen, bend );
			if ( r == std::codecvt_base::ok || r == std::codecvt_base::partial )
				blen = bend - buf;
			else if ( r == std::codecvt_base::noconv )
			{
				buf = reinterpret_cast<char *>( ibuf );
				blen = ilen;
			}
			else
				throw std::runtime_error( "Conversion error in streambuf" );

			elen = write( buf, static_cast<size_t>( blen ) );
			plen = blen;

			// try again for partial
			if ( r == std::codecvt_base::partial && elen == plen )
			{
				const char_type *iresume = iend;
				std::streamsize rlen = this->pptr() - iend;
				r = _codecvt_cache->out( _state_cur, iresume,
										 iresume + rlen, iend, buf,
										 buf + blen, bend );
				if ( r != std::codecvt_base::error )
				{
					rlen = bend - buf;
					elen = write( buf, static_cast<size_t>( rlen ) );
					plen = rlen;
				}
				else
					throw std::runtime_error( "Conversion error in streambuf" );
			}
		}
		return elen == plen;
	}

	off_type get_ext_pos( codecvt_state_type &state )
	{
		if ( _codecvt_cache->always_noconv() )
			return this->gptr() - this->egptr();

		const int gptr_off = _codecvt_cache->length(
			state, _conv_buf, _conv_next,
			static_cast<size_t>( this->gptr() - this->eback() ) );

		return _conv_buf + gptr_off - _conv_end;
	}

	pos_type
	seek_priv( off_type off, std::ios_base::seekdir dir, codecvt_state_type state )
	{
		pos_type ret = pos_type( off_type(-1) );
		if ( terminate_output() )
		{
			off_type real_off = this->seek( off, dir );
			if ( real_off != off_type(-1) )
			{
				_reading = false;
				_writing = false;
				_conv_next = _conv_end = _conv_buf;
				update_base_buffer_pointers( -1 );
				_state_cur = state;
				ret = real_off;
				ret.state( _state_cur );
			}
		}
		return ret;
	}

	bool terminate_output( void )
	{
		bool isvalid = this->sync() == 0;

		if ( _writing && ! check_facet( _codecvt_cache ).always_noconv() && isvalid )
		{
			const size_t blen = 128;
			char buf[blen];
			std::codecvt_base::result r;
			std::streamsize ilen = 0;
			do
			{
				char *next;
				r = _codecvt_cache->unshift( _state_cur, buf, buf + blen, next );
				if ( r == std::codecvt_base::error )
					isvalid = false;
				else if ( r == std::codecvt_base::ok || r == std::codecvt_base::partial )
				{
					ilen = next - buf;
					if ( ilen > 0 )
					{
						const std::streamsize elen = write( buf, static_cast<size_t>( ilen ) );
						if ( elen != ilen )
							isvalid = false;
					}
				}
			} while ( r == std::codecvt_base::partial && ilen > 0 && isvalid );

			if ( isvalid )
			{
				// this is apparently needed by the standard, but why
				// didn't overflow above handle this?
				const int_type tmp = this->overflow();
				if ( traits_type::eq_int_type( tmp, traits_type::eof() ) )
					isvalid = false;
			}
		}

		return isvalid;
	}

	std::ios_base::openmode _open_mode;
	std::streamsize _buf_sz;
	bool _writing = false;
	bool _reading = false;
	char_type *_buf = nullptr;
	std::unique_ptr<char_type[]> _buf_store;

	// for codecvt
	codecvt_state_type _state_beg;
	codecvt_state_type _state_cur;
	codecvt_state_type _state_last;
	const codecvt_type *_codecvt_cache;
	std::unique_ptr<char[]> _conv_buf_store;
	std::streamsize _conv_buf_sz = 0;
	char *_conv_buf = nullptr;
	const char *_conv_next = nullptr;
	char *_conv_end = nullptr;

	// for put back
	char_type _pback = 0;
	char_type *_pback_save_cur = nullptr;
	char_type *_pback_save_end = nullptr;
	bool _pback_live = false;

	std::string _uri;
};

typedef base_streambuf<char> streambuf;
typedef base_streambuf<wchar_t> wstreambuf;

} // namespace base



