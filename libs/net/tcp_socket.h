
#pragma once

#include "socket.h"
#include "address.h"

#include <base/uri.h>

#include <cstdint>
#include <memory>

namespace net
{

////////////////////////////////////////

class tcp_socket : public socket
{
public:
	tcp_socket( void );
	tcp_socket( tcp_socket &&that )
		: socket( std::move( that ) )
	{
	}

	tcp_socket( const socket &that ) = delete;

	void listen( int conn );

	void connect( const base::uri &host, uint16_t default_port, double timeout = 0.0 )
	{
		connect( address( host.host() ), host.port( default_port ), timeout );
	}

	void connect( uint32_t host, uint16_t port, double timeout = 0.0 );

	std::shared_ptr<tcp_socket> accept( void );

	void read( void *buf, size_t bytes );
	void write( const void *buf, size_t bytes );

	size_t bytes_waiting( void );

	void wait( void );

private:
	tcp_socket( int s );
};

////////////////////////////////////////

}

