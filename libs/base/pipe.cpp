//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "pipe.h"
#include "contract.h"
#include "utility.h"
#ifndef _WIN32
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
#endif

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

pipe::pipe( bool isPrivate, bool blocking )
{
#ifdef _WIN32
	_p[0] = INVALID_HANDLE_VALUE;
	_p[1] = INVALID_HANDLE_VALUE;
	if ( ! CreatePipe( &_p[0], &_p[1], NULL, 0 ) )
		throw_lasterror( "Unable to create pipe object" );
#else
	_p[0] = -1;
	_p[1] = -1;
	int flags = 0;
	if ( isPrivate )
		flags = O_CLOEXEC;
	if ( ! blocking )
		flags |= O_NONBLOCK;

	int r = ::pipe2( _p, flags );
	if ( r == -1 )
		throw_errno( "Unable to create pipe object" );
#endif
}

////////////////////////////////////////

pipe::~pipe( void )
{
	shutdownRead();
	shutdownWrite();
}

////////////////////////////////////////

pipe::pipe( pipe &&o )
{
#ifdef _WIN32
	_p[0] = exchange( o._p[0], INVALID_HANDLE_VALUE );
	_p[1] = exchange( o._p[1], INVALID_HANDLE_VALUE );
#else
	_p[0] = exchange( o._p[0], -1 );
	_p[1] = exchange( o._p[1], -1 );
#endif
}

////////////////////////////////////////

pipe &
pipe::operator=( pipe &&o )
{
	if ( this != &o )
	{
		std::swap( _p[0], o._p[0] );
		std::swap( _p[1], o._p[1] );
	}
	return *this;
}

////////////////////////////////////////

intptr_t
pipe::readable( void ) const
{
#ifdef _WIN32
	return reinterpret_cast<intptr_t>( _p[0] );
#else
	return _p[0];
#endif
}

////////////////////////////////////////

intptr_t
writable( void ) const
{
#ifdef _WIN32
	return reinterpret_cast<intptr_t>( _p[1] );
#else
	return _p[1];
#endif
}

////////////////////////////////////////

ssize_t
pipe::read( void *d, size_t n )
{
	ssize_t nr = 0;
#ifdef _WIN32
	if ( readable() == INVALID_HANDLE_VALUE )
		return -1;
	DWORD nToR = static_cast<DWORD>( n );
	DWORD nar = 0;
	if ( ReadFile( readable(), d, nToR, &nar, NULL ) )
	{
		nr = static_cast<ssize_t>( nar );
	}
	else
		throw_lasterror( "Unable to read {0} bytes from the pipe", n );
#else
	if ( readable() < 0 )
		return -1;

	ssize_t nr = 0;
	uint8_t *dPtr = reinterpret_cast<uint8_t *>( d );
	while ( n > 0 )
	{
		ssize_t nar = ::read( readable(), dPtr, n );
		if ( nar < 0 )
		{
			if ( errno == EINTR )
				continue;
			throw_errno( "Unable to read {0} bytes from the pipe", n );
		}
		if ( nar == 0 )
			break;
		nr += nar;
		n -= nar;
		dPtr += nar;
	}
#endif

	return nr;
}

////////////////////////////////////////

ssize_t
pipe::write( const void *d, size_t n )
{
	ssize_t totW = 0;
#ifdef _WIN32
	if ( writable() == INVALID_HANDLE_VALUE )
		return -1;
	DWORD nToW = static_cast<DWORD>( n );
	DWORD naw = 0;
	if ( WriteFile( writable(), d, nToW, &naw, NULL ) )
	{
		totW = static_cast<ssize_t>( naw );
	}
	else
		throw_lasterror( "Unable to read {0} bytes from the pipe", n );
#else
	if ( writable() < 0 )
		return -1;

	const uint8_t *dPtr = reinterpret_cast<const uint8_t *>( d );
	while ( n > 0 )
	{
		ssize_t nw = ::write( writable(), dPtr, n );
		if ( nw < 0 )
		{
			if ( errno == EINTR )
				continue;
			throw_errno( "Unable to write {0} bytes to the pipe", n );
		}
		n -= nw;
		dPtr += nw;
		totW += nw;
	}
#endif

	return totW;
}

////////////////////////////////////////

void
pipe::shutdownRead( void )
{
#ifdef _WIN32
	if ( _p[0] != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( _p[0] );
		_p[0] = INVALID_HANDLE_VALUE;
	}
#else
	if ( _p[0] >= 0 )
	{
		::close( _p[0] );
		_p[0] = -1;
	}
#endif
}

////////////////////////////////////////

void
pipe::shutdownWrite( void )
{
#ifdef _WIN32
	if ( _p[1] != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( _p[1] );
		_p[1] = INVALID_HANDLE_VALUE;
	}
#else
	if ( _p[1] >= 0 )
	{
		::close( _p[1] );
		_p[1] = -1;
	}
#endif
}

////////////////////////////////////////

} // namespace base



