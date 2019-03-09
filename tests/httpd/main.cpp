// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include <base/uri.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <web/server.h>
#include <web/socket.h>

std::string hello( const std::string &n );
std::string websocket( void );

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	web::server test( 8080, 10 );
	test.default_resource( "GET" ) = []( web::request &req, net::tcp_socket &client )
	{
		std::cerr << "404 NOT FOUND " << req.path().pretty() << std::endl;
		web::server::not_found( req, client );
	};

	test.resource( "GET", "/hello" ) = []( web::request &, net::tcp_socket &client )
	{
		web::response resp( hello( "World" ) );
		resp.send( client );
	};

	test.resource( "GET", "/socket" ) = []( web::request &, net::tcp_socket &client )
	{
		web::response resp( websocket() );
		resp.send( client );
	};

	test.resource( "GET", "/ws" ) = []( web::request &req, net::tcp_socket &client )
	{
		web::socket ws( req, std::move( client ) );
		ws.when_message.connect( [&]( const std::string &msg, bool )
		{
			std::cout << "GOT MESSAGE: " << msg << std::endl;
			ws.send( msg );
		} );

		ws.run();
	};

	test.resource( "GET", "/exception" ) = []( web::request &, net::tcp_socket & )
	{
		throw_runtime( "Ooops, error" );
	};

	test.run();

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
