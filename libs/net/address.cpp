// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "address.h"
#include <base/contract.h>
#include <cstring>
#ifdef _WIN32
# include <ws2tcpip.h>
#else
# include <netdb.h>
#endif

namespace net
{

////////////////////////////////////////

void address::lookup_name( const char *name )
{
	precondition( name, "null lookup name" );
	precondition( name[0] != '\0', "empty lookup name" );

    struct addrinfo hints;
    ::memset( &hints, 0, sizeof(hints) );
    hints.ai_family = AF_INET;

    struct addrinfo *res0 = NULL;
    int err = EAI_AGAIN;
    while ( err == EAI_AGAIN )
        err = getaddrinfo( name, NULL, &hints, &res0 );

    if ( err == 0 )
    {
        struct addrinfo *res = res0;
        for ( ; res; res=res->ai_next )
        {
            if ( res->ai_addrlen == sizeof(struct sockaddr_in) )
                break;
        }

        if ( res )
        {
            struct sockaddr_in *a = reinterpret_cast<struct sockaddr_in *>( res->ai_addr );
            _addr = ntohl( a->sin_addr.s_addr );
        }

        freeaddrinfo( res0 );

        if ( !res )
            throw_runtime( "no valid address found" );
    }
    else
        throw_runtime( "lookup name {0}: {1}", name, gai_strerror( err ) );
}

////////////////////////////////////////

}

