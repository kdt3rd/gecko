
#include "server.h"
#include <base/thread_pool.h>
#include <base/scope_guard.h>
#include <net/tcp_socket.h>
#include <regex>

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
	socket.listen( 5 );

	while ( !_done )
	{
		auto client = socket.accept();
		threads.queue( [=]() { this->handle_client( *client ); } );
	}
}

////////////////////////////////////////

void server::handle_client( net::tcp_socket &client )
{
	std::cout << "CLIENT CONNECT" << std::endl;
	request req( client );
	const auto &resources = _resources.find( req.method() );
	if ( resources != _resources.end() )
	{
		std::string path = req.path().full_path();
		for ( auto &r: resources->second )
		{
			std::regex re( r.first );
			if ( std::regex_match( path, re ) )
			{
				response resp;
				r.second( resp, req );
				resp.send( client );
				return;
			}
		}
	}

	const auto &handler = _defaults.find( req.method() );
	if ( handler != _defaults.end() )
	{
		response resp;
		handler->second( resp, req );
		resp.send( client );
		return;
	}

	response resp;
	resp.send( client );
}

////////////////////////////////////////

}


