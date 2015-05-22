
#pragma once

#include "request.h"
#include <base/signal.h>

namespace web
{

////////////////////////////////////////

class socket
{
public:
	/// @brief Open a WebSocket from the request
	socket( request &req, net::tcp_socket &&client );
	~socket( void );

	base::signal<void(void)> on_open;
	base::signal<void(const std::string &,bool)> on_message;
	base::signal<void(void)> on_close;

	void send( const std::string &msg );

	void run( void );

protected:
	bool _masked = false;
	net::tcp_socket _client;
};

////////////////////////////////////////

}

