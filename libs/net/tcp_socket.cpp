// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <base/scope_guard.h>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#ifdef _WIN32
#    include <winsock2.h>
#    include <ws2tcpip.h>
#else
#    include <netinet/in.h>
#    include <netinet/ip.h>
#    include <netinet/tcp.h>
#    include <sys/ioctl.h>
#    include <sys/select.h>
#    include <sys/socket.h>
#    include <sys/types.h>
#endif

#include "address.h"
#include "tcp_socket.h"

namespace net
{
////////////////////////////////////////

tcp_socket::tcp_socket( void )
{
    _socket = ::socket( AF_INET, SOCK_STREAM, 0 );
    if ( _socket < 0 )
        throw_errno( "creating socket" );

#ifdef _WIN32
    // grrr, windows has these same things, but they take different types...
    // double grrr, they also take a char * instead of void *
    // for the argument thing, but we'll just cheat and do the
    // cast everywhere
    BOOL on = true;
#else
    int on = 1;
#endif

    if ( setsockopt(
             _socket,
             SOL_SOCKET,
             SO_REUSEADDR,
             reinterpret_cast<const char *>(&on),
             sizeof( on ) ) < 0 )
        throw_errno( "setsockopt/reuseaddr" );
    if ( setsockopt(
             _socket,
             SOL_SOCKET,
             SO_KEEPALIVE,
             reinterpret_cast<const char *>(&on),
             sizeof( on ) ) < 0 )
        throw_errno( "setsockopt/keepalive" );
    if ( setsockopt(
             _socket,
             IPPROTO_TCP,
             TCP_NODELAY,
             reinterpret_cast<const char *>(&on),
             sizeof( on ) ) < 0 )
        throw_errno( "setsockopt/nodelay" );
#ifdef __linux__
    if ( setsockopt( _socket, IPPROTO_TCP, TCP_CORK, &on, sizeof( on ) ) < 0 )
        throw_errno( "setsockopt/cork" );
#endif

#ifndef _WIN32
    int low_delay = IPTOS_LOWDELAY;
    if ( setsockopt(
             _socket, IPPROTO_IP, IP_TOS, &low_delay, sizeof( low_delay ) ) <
         0 )
        throw_errno( "setsockopt/lowdelay" );
#endif
}

////////////////////////////////////////

void tcp_socket::listen( int conn )
{
    if ( ::listen( _socket, conn ) )
        throw_errno( "TCP socket listen" );
}

////////////////////////////////////////

void tcp_socket::connect( uint32_t host, uint16_t port, double timeout )
{
    // setup remote address.
    struct sockaddr_in remote;
    ::memset( &remote, 0, sizeof( remote ) );
    remote.sin_family      = AF_INET;
    remote.sin_addr.s_addr = htonl( host );
    remote.sin_port        = htons( port );

#ifdef _WIN32
#    pragma TODO( "Win32 has ioctlsocket but unclear how to check the error" )
//	unsigned long mode = 1;
//	ioctlsocket( _socket, FIONBIO, &mode );
//	on_scope_exit
//	{
//		mode = 0;
//		ioctlsocket( _socket, FIONBIO, &mode );
//	};
#else
    // switch to non-blocking (and back when done).
    int flags = fcntl( _socket, F_GETFL, 0 );
    fcntl( _socket, F_SETFL, flags | O_NONBLOCK );
    on_scope_exit { fcntl( _socket, F_SETFL, flags ); };
#endif

    // begin connect and wait for completion.
    if ( ::connect(
             _socket,
             reinterpret_cast<struct sockaddr *>( &remote ),
             sizeof( remote ) ) == -1 )
    {
        if ( errno != EINPROGRESS )
            throw_errno( "connecting to {0}:{1}", host, port );

        fd_set readset;
        FD_ZERO( &readset );
        FD_SET( _socket, &readset );
        fd_set writeset = readset;

        struct timeval tval;
        tval.tv_sec  = static_cast<int>( std::floor( timeout ) );
        tval.tv_usec = static_cast<int>( std::floor(
            ( timeout - static_cast<double>( tval.tv_sec ) ) * 1000000.0 ) );

        int count = ::select(
            _socket + 1,
            &readset,
            &writeset,
            nullptr,
            ( timeout > 0.0 ) ? &tval : nullptr );

        if ( count < 0 )
            throw_errno( "connect/select" );

        if ( count == 0 )
            throw_location( std::system_error(
                ETIMEDOUT, std::system_category(), "connect" ) );

#ifndef _WIN32
        int       err    = -1;
        socklen_t errlen = sizeof( err );
        if ( ::getsockopt( _socket, SOL_SOCKET, SO_ERROR, &err, &errlen ) != 0 )
            throw_errno( "connect/getsockopt" );

        if ( err != 0 )
            throw_location(
                std::system_error( err, std::system_category(), "connect" ) );
#endif
    }
}

////////////////////////////////////////

std::shared_ptr<tcp_socket> tcp_socket::accept( void )
{
    struct sockaddr_in clientaddr;

    int       socket = -1;
    socklen_t len    = sizeof( clientaddr );
    while ( ( socket = ::accept(
                  _socket,
                  reinterpret_cast<struct sockaddr *>( &clientaddr ),
                  &len ) ) < 0 )
    {
        if ( errno != EINTR && errno != ECONNABORTED )
            throw_errno( "TCP socket accept failed" );
    }

    return std::unique_ptr<tcp_socket>( new tcp_socket( socket ) );
}

////////////////////////////////////////

void tcp_socket::read( void *buf, size_t bytes )
{
    uint8_t *mem = static_cast<uint8_t *>( buf );

    size_t  nleft = bytes;
    ssize_t nread;

    while ( nleft > 0 )
    {
        nread = ::read( _socket, mem, nleft );
        if ( nread < 0 )
        {
            if ( errno != EINTR )
                throw_errno( "reading TCP socket {0}", _socket );
            continue;
        }
        if ( nread == 0 )
            throw_location( std::system_error(
                ECONNABORTED,
                std::system_category(),
                base::format( "TCP socket read {0}", _socket ) ) );
        nleft -= static_cast<size_t>( nread );
        mem += nread;
    }
}

////////////////////////////////////////

void tcp_socket::write( const void *buf, size_t bytes )
{
    const uint8_t *mem = reinterpret_cast<const uint8_t *>( buf );

    size_t  nleft = bytes;
    ssize_t nwrite;

    while ( nleft > 0 )
    {
        nwrite = ::write( _socket, mem, nleft );
        if ( nwrite < 0 )
        {
            int e = errno;
            if ( e == EINTR )
                continue;

            if ( e == EAGAIN )
            {
                fd_set fds;
                FD_ZERO( &fds );
                FD_SET( _socket, &fds );
                struct timeval tval;
#ifdef _WIN32
                tval.tv_sec  = 0;
                tval.tv_usec = 0;
#else
                // windows does not support the SO_SNDTIMEO, although they do support setting it?
                socklen_t tSz = sizeof( tval );
                if ( ::getsockopt(
                         _socket, SOL_SOCKET, SO_SNDTIMEO, &tval, &tSz ) != 0 )
                    throw_errno( "getsockopt/sndtimeo" );
#endif
                while ( true )
                {
                    int s = 0;
                    if ( tval.tv_sec == 0 && tval.tv_usec == 0 )
                        s = select( _socket + 1, nullptr, &fds, nullptr, nullptr );
                    else
                        s = select( _socket + 1, nullptr, &fds, nullptr, &tval );

                    if ( s == -1 && errno == EINTR )
                        continue;
                    if ( s == -1 )
                        throw_errno( "write/writewait" );

                    if ( s == 0 )
                        throw_errno( "write/systemswamp" );

                    break;
                }
                continue;
            }
            throw_errno( "write" );
        }

        nleft -= static_cast<size_t>( nwrite );
        mem += nwrite;
    }
}

////////////////////////////////////////

size_t tcp_socket::bytes_waiting( void )
{
#ifdef _WIN32
    u_long n = 0;
    if ( ioctlsocket( _socket, FIONREAD, &n ) != 0 )
        throw_lasterror( "bytes/ioctl" );
#else
    int n = 0;
    if ( ioctl( _socket, FIONREAD, &n ) != 0 )
        throw_errno( "bytes/ioctl" );
#endif
    return static_cast<size_t>( n );
}

////////////////////////////////////////

void tcp_socket::wait( void )
{
    fd_set readset;
    FD_ZERO( &readset );
    FD_SET( _socket, &readset );

    int count = ::select( _socket + 1, &readset, nullptr, nullptr, nullptr );
    if ( count < 0 )
        throw_errno( "wait/select" );
}

////////////////////////////////////////

tcp_socket::tcp_socket( int s ) { _socket = s; }

////////////////////////////////////////

} // namespace net
