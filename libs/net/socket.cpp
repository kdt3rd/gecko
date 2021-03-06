// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "socket.h"

#include <base/contract.h>
#include <cstring>
#include <unistd.h>
#ifdef _WIN32
#    include <winsock2.h>
#else
#    include <netinet/in.h>
#    include <netinet/ip.h>
#    include <sys/socket.h>
#endif

namespace net
{
////////////////////////////////////////

socket::socket( void ) {}

////////////////////////////////////////

socket::~socket( void )
{
    if ( _socket >= 0 )
        ::close( _socket );
    _socket = -1;
}

////////////////////////////////////////

void socket::bind( uint16_t port ) { bind( INADDR_ANY, port ); }

////////////////////////////////////////

void socket::bind( uint32_t host, uint16_t port )
{
    precondition( _socket >= 0, "invalid socket" );
    struct sockaddr_in local;
    ::memset( &local, 0, sizeof( local ) );
    local.sin_family      = AF_INET;
    local.sin_addr.s_addr = htonl( host );
    local.sin_port        = htons( port );

    if ( ::bind(
             _socket,
             reinterpret_cast<struct sockaddr *>( &local ),
             sizeof( local ) ) == -1 )
        throw_errno( "socking binding to {0}:{1}", host, port );
}

////////////////////////////////////////

void socket::close( void )
{
    if ( _socket >= 0 )
        ::close( _socket );
    _socket = -1;
}

////////////////////////////////////////

} // namespace net
