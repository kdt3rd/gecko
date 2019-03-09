// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "client.h"
#include "request.h"
#include "response.h"

#include <net/tcp_socket.h>

namespace web
{

////////////////////////////////////////

client::client( std::string agent )
	: _agent( std::move( agent ) )
{
}

////////////////////////////////////////

response client::get( const base::uri &path, double timeout )
{
	precondition( path.scheme() == "http", "unsupported scheme: {0}", path.pretty() );

	request req( "GET", path );
	req.set_header( "User-Agent", _agent );
	req.set_header( "Host", path.host() + ":" + base::to_string( path.port( 80 ) ) );

	net::tcp_socket server;
	server.connect( path, 80, timeout );
	req.send( server );

	response result( server );
	return result;
}

////////////////////////////////////////

response client::post( const base::uri &path, std::string &&v, double timeout )
{
	precondition( path.scheme() == "http", "unsupported scheme: {0}", path.pretty() );

	request req( "POST", path );
	req.set_header( "User-Agent", _agent );
	req.set_header( "Host", path.host() + ":" + base::to_string( path.port( 80 ) ) );
	req.set_content( std::move( v ) );

	net::tcp_socket server;
	server.connect( path, 80, timeout );
	req.send( server );

	response result( server );
	return result;
}

////////////////////////////////////////

}

