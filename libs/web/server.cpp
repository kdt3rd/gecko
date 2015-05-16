
#include "server.h"
#include <base/thread_pool.h>
#include <base/scope_guard.h>
#include <net/tcp_socket.h>

namespace web
{

server::server( uint16_t port, size_t threads )
	: _port( port ), _threads( threads )
{
}

////////////////////////////////////////

server::handler &server::resource( const std::string &method, const std::string &re )
{
	return _resources[method][re];
}

////////////////////////////////////////

server::handler &server::default_resource( const std::string &method )
{
	return _defaults[method];
}

////////////////////////////////////////

void server::run( void )
{
	_done = false;

	base::thread_pool threads( _threads );

	net::tcp_socket socket;
	socket.bind( _port );

	while ( !_done )
	{
		auto client = socket.accept();
		threads.queue( [=]() { this->handle_client( *client ); } );
	}
}

////////////////////////////////////////

void server::handle_client( net::tcp_socket &client )
{
	// Read request and process it...
}

////////////////////////////////////////

}


