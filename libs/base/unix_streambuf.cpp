// Copyright (c) 2015 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "unix_streambuf.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <poll.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace base
{
////////////////////////////////////////

unix_streambuf::unix_streambuf(
    std::ios_base::openmode m,
    int                     fd,
    bool                    doDup,
    const std::string &     path,
    std::streamsize         bufSz )
    : streambuf( m, bufSz ), _fd( -1 ), _path( path )
{
    if ( doDup )
    {
        int nfd = dup( fd );
        while ( nfd < 0 )
        {
            if ( errno != EINTR || errno != EBUSY )
            {
                if ( path.empty() )
                    throw_errno( "duplicating file descriptor" );
                else
                    throw_errno( "duplicating file descriptor ({0})", path );
            }
            nfd = dup( fd );
        }
        _fd = nfd;
    }
    else
        _fd = fd;

    initFD( m );
}

////////////////////////////////////////

unix_streambuf::unix_streambuf(
    std::ios_base::openmode m, const uri &path, std::streamsize bufSz )
    : streambuf( m, bufSz ), _path( path.full_path() )
{
    stash_uri( path.pretty() );
    initFD( m );
}

////////////////////////////////////////

unix_streambuf::unix_streambuf(
    std::ios_base::openmode m, const std::string &path, std::streamsize bufSz )
    : streambuf( m, bufSz ), _path( path )
{
    initFD( m );
}

////////////////////////////////////////

unix_streambuf::unix_streambuf(
    std::ios_base::openmode m, std::string &&path, std::streamsize bufSz )
    : streambuf( m, bufSz ), _path( std::move( path ) )
{
    initFD( m );
}

////////////////////////////////////////

unix_streambuf::unix_streambuf( unix_streambuf &&u )
    : streambuf( std::move( u ) )
    , _fd( base::exchange( u._fd, -1 ) )
    , _path( std::move( u._path ) )
{}

////////////////////////////////////////

unix_streambuf &unix_streambuf::operator=( unix_streambuf &&u )
{
    streambuf::operator=( std::move( u ) );
    _fd                = base::exchange( u._fd, -1 );
    _path              = std::move( u._path );
    return *this;
}

////////////////////////////////////////

unix_streambuf::~unix_streambuf( void ) { this->close(); }

////////////////////////////////////////

void unix_streambuf::swap( unix_streambuf &u )
{
    streambuf::swap( u );
    std::swap( _fd, u._fd );
    std::swap( _path, u._path );
}

////////////////////////////////////////

bool unix_streambuf::is_open( void ) const { return _fd >= 0; }

////////////////////////////////////////

void unix_streambuf::close( void ) noexcept
{
    if ( _fd >= 0 )
    {
        ::close( _fd );
        _fd = -1;
    }
}

////////////////////////////////////////

unix_streambuf::off_type unix_streambuf::bytes_avail( void )
{
    if ( _fd < 0 )
        return 0;

    int n   = 0;
    int ret = ioctl( _fd, FIONREAD, &n );
    if ( ret == 0 && n >= 0 )
        return n;

    struct pollfd __pfd[1];
    __pfd[0].fd     = _fd;
    __pfd[0].events = POLLIN;
    if ( poll( __pfd, 1, 0 ) <= 0 )
        return 0;

    struct stat statbuf;
    ret = fstat( _fd, &statbuf );
    if ( ret == 0 && S_ISREG( statbuf.st_mode ) )
    {
        const std::streamoff off =
            statbuf.st_size - lseek( _fd, 0, std::ios_base::cur );
        return std::min(
            off,
            std::streamoff( std::numeric_limits<std::streamsize>::max() ) );
    }

    return 0;
}

////////////////////////////////////////

unix_streambuf::off_type
unix_streambuf::seek( off_type off, std::ios_base::seekdir dir )
{
    off_type ret = -1;
    if ( _fd >= 0 )
        ret = ::lseek( _fd, off, dir );
    return ret;
}

////////////////////////////////////////

std::streamsize unix_streambuf::read( void *outBuf, size_t numBytes )
{
    std::streamsize ret = -1;
    if ( _fd >= 0 )
    {
        do
        {
            ret = ::read( _fd, outBuf, numBytes );
        } while ( ret == -1 && errno == EINTR );
    }
    return ret;
}

////////////////////////////////////////

std::streamsize unix_streambuf::write( const void *outBuf, size_t numBytes )
{
    size_t nleft = numBytes;

    if ( _fd < 0 )
        return 0;

    const char *s = reinterpret_cast<const char *>( outBuf );
    do
    {
        const ssize_t ret = ::write( _fd, s, nleft );
        if ( ret == -1 )
        {
            if ( errno == EINTR )
                continue;
            break;
        }

        nleft -= static_cast<size_t>( ret );
        if ( nleft == 0 )
            break;

        s += ret;
    } while ( true );

    return static_cast<std::streamsize>( numBytes - nleft );
}

////////////////////////////////////////

std::streamsize unix_streambuf::writev(
    const void *outBuf1,
    size_t      numBytes1,
    const void *outBuf2,
    size_t      numBytes2 )
{
    const char *s1 = reinterpret_cast<const char *>( outBuf1 );
    const char *s2 = reinterpret_cast<const char *>( outBuf2 );

    size_t nleft   = numBytes1 + numBytes2;
    size_t n1_left = numBytes1;

    struct iovec iov[2];
    iov[1].iov_base = const_cast<char *>( s2 );
    iov[1].iov_len  = numBytes2;

    while ( true )
    {
        iov[0].iov_base = const_cast<char *>( s1 );
        iov[0].iov_len  = n1_left;

        std::streamsize ret = ::writev( _fd, iov, 2 );
        if ( ret == -1 )
        {
            if ( errno == EINTR )
                continue;
            break;
        }

        nleft -= static_cast<size_t>( ret );
        if ( nleft == 0 )
            break;

        const std::streamsize off =
            ret - static_cast<std::streamsize>( n1_left );
        if ( off >= 0 )
        {
            // only have the second buffer left, just do a normal write instead of
            // writev
            ret =
                this->write( s2 + off, numBytes2 - static_cast<size_t>( off ) );
            nleft -= static_cast<size_t>( ret );
            break;
        }

        s1 += ret;
        n1_left -= static_cast<size_t>( ret );
    }

    return static_cast<std::streamsize>( numBytes1 + numBytes2 - nleft );
}

////////////////////////////////////////

void unix_streambuf::initFD( std::ios_base::openmode m )
{
    if ( _fd == -1 )
    {
        if ( _path.empty() )
            throw_logic( "invalid path to open file" );

        int flags = 0;

        std::ios_base::openmode mnoate = m & ~std::ios_base::ate;

        // under unix, don't care about the binary flag either
        mnoate = mnoate & ~std::ios_base::binary;

        if ( mnoate == std::ios_base::in )
            flags = O_RDONLY;
        else if (
            mnoate == std::ios_base::out ||
            mnoate == ( std::ios_base::out | std::ios_base::trunc ) )
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        else if (
            mnoate == std::ios_base::app ||
            mnoate == ( std::ios_base::out | std::ios_base::app ) )
            flags = O_WRONLY | O_APPEND;
        else if ( mnoate == ( std::ios_base::in | std::ios_base::out ) )
            flags = O_RDWR;
        else if (
            mnoate ==
            ( std::ios_base::in | std::ios_base::out | std::ios_base::trunc ) )
            flags = O_RDWR | O_CREAT | O_TRUNC;
        else if (
            mnoate == ( std::ios_base::in | std::ios_base::out |
                        std::ios_base::app ) ||
            mnoate == ( std::ios_base::in | std::ios_base::app ) )
            flags = O_RDWR | O_APPEND | O_CREAT;

        mode_t mode = ( S_IRUSR | S_IWUSR ) | ( S_IRGRP | S_IWGRP ) |
                      ( S_IROTH | S_IWOTH );
        int nfd = ::open( _path.c_str(), flags, mode );
        if ( nfd < 0 )
            throw_runtime( "unable to open '{0}'", _path );
        _fd = nfd;
    }

    if ( m & std::ios_base::ate )
    {
        if ( this->seek( 0, std::ios_base::end ) == -1 )
            throw_runtime( "unable to seek to the end of the stream" );
    }
}

////////////////////////////////////////

} // namespace base
