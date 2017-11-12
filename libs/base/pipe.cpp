//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "pipe.h"
#include "contract.h"
#include "utility.h"
#ifdef _WIN32
# include <windows.h>
#else
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
	_p[0] = -1;
	_p[1] = -1;
#ifdef _WIN32
	XXXXXX_TODO_XXXXXXXX;
#else
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
	if ( _p[0] >= 0 )
		::close( _p[0] );
	if ( _p[1] >= 0 )
		::close( _p[1] );
}

////////////////////////////////////////

pipe::pipe( pipe &&o )
{
	_p[0] = exchange( o._p[0], -1 );
	_p[1] = exchange( o._p[1], -1 );
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

ssize_t
pipe::read( void *d, size_t n )
{
	if ( readable() < 0 )
		return -1;

	ssize_t nr = 0;
	uint8_t *dPtr = reinterpret_cast<uint8_t *>( d );
	while ( n > 0 )
	{
		ssize_t nar = ::read( readable(), dPtr, n );
		if ( nr < 0 )
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

	return nr;
}

////////////////////////////////////////

ssize_t
pipe::write( const void *d, size_t n )
{
	if ( writable() < 0 )
		return -1;

	const uint8_t *dPtr = reinterpret_cast<const uint8_t *>( d );
	ssize_t totW = 0;
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
	return totW;
}

////////////////////////////////////////

void
pipe::shutdownRead( void )
{
	if ( _p[0] >= 0 )
	{
		::close( _p[0] );
		_p[0] = -1;
	}
}

////////////////////////////////////////

void
pipe::shutdownWrite( void )
{
	if ( _p[1] >= 0 )
	{
		::close( _p[1] );
		_p[1] = -1;
	}
}

////////////////////////////////////////

} // namespace base



