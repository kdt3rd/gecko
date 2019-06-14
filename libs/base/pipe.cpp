// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "pipe.h"

#include "contract.h"
#include "utility.h"
#ifndef _WIN32
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

pipe::pipe( bool isPrivate, bool blocking )
{
    _p[0] = wait::INVALID_WAIT;
    _p[1] = wait::INVALID_WAIT;
#ifdef _WIN32
    HANDLE rh = INVALID_HANDLE_VALUE;
    HANDLE wh = INVALID_HANDLE_VALUE;
    if ( !CreatePipe( &rh, &wh, NULL, 0 ) )
        throw_lasterror( "Unable to create pipe object" );
    _p[0] = (handle)rh;
    _p[1] = (handle)wh;
#else
    int flags = 0;
    if ( isPrivate )
        flags = O_CLOEXEC;
    if ( !blocking )
        flags |= O_NONBLOCK;

#    ifdef __linux__
    int r = ::pipe2( _p, flags );
    if ( r == -1 )
        throw_errno( "Unable to create pipe object" );
#    else
    // TODO: thread safety if someone fork/execs while we're opening?
    int r = ::pipe( _p );
    if ( r == -1 )
        throw_errno( "Unable to create pipe object" );
    if ( flags != 0 )
    {
        r = fcntl( _p[0], F_SETFL, flags );
        if ( r == -1 )
            throw_errno( "Unable to set flags on pipe object" );
        r = fcntl( _p[1], F_SETFL, flags );
        if ( r == -1 )
            throw_errno( "Unable to set flags on pipe object" );
    }
#    endif
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
    // NB: until c++17, avoid ODR usage of const / constexpr value
    // (taking a reference), which forces it to be defined in a
    // translation unit somewhere for linking...
#if __cplusplus > 201402L
    _p[0] = exchange( o._p[0], wait::INVALID_WAIT );
    _p[1] = exchange( o._p[1], wait::INVALID_WAIT );
#else
    _p[0] = exchange( o._p[0], wait::wait_type( wait::INVALID_WAIT ) );
    _p[1] = exchange( o._p[1], wait::wait_type( wait::INVALID_WAIT ) );
#endif
}

////////////////////////////////////////

pipe &pipe::operator=( pipe &&o )
{
    if ( this != &o )
    {
        std::swap( _p[0], o._p[0] );
        std::swap( _p[1], o._p[1] );
    }
    return *this;
}

////////////////////////////////////////

ssize_t pipe::read( void *d, size_t n )
{
    ssize_t nr = 0;
#ifdef _WIN32
    if ( _p[0] == wait::INVALID_WAIT )
        return -1;
    DWORD nToR = static_cast<DWORD>( n );
    DWORD nar  = 0;
    if ( ReadFile( (HANDLE)_p[0], d, nToR, &nar, NULL ) )
    {
        nr = static_cast<ssize_t>( nar );
    }
    else
        throw_lasterror( "Unable to read {0} bytes from the pipe", n );
#else
    if ( _p[0] < 0 )
        return -1;

    uint8_t *dPtr = reinterpret_cast<uint8_t *>( d );
    while ( n > 0 )
    {
        ssize_t nar = ::read( _p[0], dPtr, n );
        if ( nar < 0 )
        {
            if ( errno == EINTR )
                continue;
            throw_errno( "Unable to read {0} bytes from the pipe", n );
        }
        if ( nar == 0 )
            break;
        nr += nar;
        n -= static_cast<size_t>( nar );
        dPtr += nar;
    }
#endif

    return nr;
}

////////////////////////////////////////

ssize_t pipe::write( const void *d, size_t n )
{
    ssize_t totW = 0;
#ifdef _WIN32
    if ( _p[1] == wait::INVALID_WAIT )
        return -1;
    DWORD nToW = static_cast<DWORD>( n );
    DWORD naw  = 0;
    if ( WriteFile( (HANDLE)_p[1], d, nToW, &naw, NULL ) )
    {
        totW = static_cast<ssize_t>( naw );
    }
    else
        throw_lasterror( "Unable to read {0} bytes from the pipe", n );
#else
    if ( _p[1] < 0 )
        return -1;

    const uint8_t *dPtr = reinterpret_cast<const uint8_t *>( d );
    while ( n > 0 )
    {
        ssize_t nw = ::write( _p[1], dPtr, n );
        if ( nw < 0 )
        {
            if ( errno == EINTR )
                continue;
            throw_errno( "Unable to write {0} bytes to the pipe", n );
        }
        n -= static_cast<size_t>( nw );
        dPtr += nw;
        totW += nw;
    }
#endif

    return totW;
}

////////////////////////////////////////

void pipe::shutdownRead( void )
{
#ifdef _WIN32
    if ( _p[0] != wait::INVALID_WAIT )
    {
        ::CloseHandle( (HANDLE)_p[0] );
        _p[0] = wait::INVALID_WAIT;
    }
#else
    if ( _p[0] != wait::INVALID_WAIT )
    {
        ::close( _p[0] );
        _p[0] = wait::INVALID_WAIT;
    }
#endif
}

////////////////////////////////////////

void pipe::shutdownWrite( void )
{
#ifdef _WIN32
    if ( _p[1] != wait::INVALID_WAIT )
    {
        ::CloseHandle( (HANDLE)_p[1] );
        _p[1] = wait::INVALID_WAIT;
    }
#else
    if ( _p[1] != wait::INVALID_WAIT )
    {
        ::close( _p[1] );
        _p[1] = wait::INVALID_WAIT;
    }
#endif
}

////////////////////////////////////////

} // namespace base
