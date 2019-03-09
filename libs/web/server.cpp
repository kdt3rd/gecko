// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

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
	_defaults["GET"] = &not_found;
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
		threads.queue( [=]()
		{
			try
			{
				this->handle_client( *client );
			}
			catch ( const std::exception &e )
			{
				try
				{
					response resp( "<!DOCTYPE html><html lang=\"en\"><head><title>Internal Server Error</title></head><body>Internal Server Error</body></html>" );
					resp.set_status_code( status_code::INTERNAL_ERROR );
					resp.send( *client );
				}
				catch ( ... )
				{
				}
				base::print_exception( std::cerr, e );
			}
		} );
	}
}

////////////////////////////////////////

void server::not_found( request &, net::tcp_socket &client )
{
	response resp( "<!DOCTYPE html><html lang=\"en\"><head><title>Not Found</title></head><body>404 Not Found</body></html>" );
	resp.set_status_code( status_code::NOT_FOUND );
	resp.send( client );
}

////////////////////////////////////////

void server::handle_client( net::tcp_socket &client )
{
	// TODO support keep-alive connection properly
	while ( true )
	{
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
					r.second( req, client );
					return;
				}
			}
		}

		const auto &handler = _defaults.find( req.method() );
		if ( handler != _defaults.end() )
		{
			handler->second( req, client );
			return;
		}

		response resp;
		resp.set_status_code( status_code::METHOD_NOT_ALLOWED );
		resp.send( client );
	}
}

////////////////////////////////////////

}


