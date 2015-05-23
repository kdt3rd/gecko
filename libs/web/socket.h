
#pragma once

#include "request.h"
#include <base/signal.h>
#include <random>

namespace web
{

////////////////////////////////////////

class socket
{
public:
	/// @brief Open a WebSocket from a request
	socket( request &req, net::tcp_socket &&client );

	/// @brief Open a WebSocket to a server
	socket( base::uri &server, const std::string &agent = "WSClient/0.0", double timeout = 0.0 );

	~socket( void );

	base::signal<void(const std::string &,bool)> when_message;

	void send( const std::string &msg );

	void run( void );

	void close( void );

protected:
	uint8_t random_byte( void )
	{
		return _dist8( _rand );
	}

	bool _masked = false;
	bool _closed = false;
	net::tcp_socket _socket;
	std::random_device _rand;
	std::uniform_int_distribution<uint8_t> _dist8;
};

////////////////////////////////////////

}

