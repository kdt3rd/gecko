
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

}

