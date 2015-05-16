
#include "response.h"

namespace
{
	std::string read_line( net::tcp_socket &socket )
	{
		char c = '\0';
		socket.read( &c, 1 );

		std::string result;
		while ( c != '\r' )
		{
			result.push_back( c );
			socket.read( &c, 1 );
		}
		socket.read( &c, 1 );
		if ( c != '\n' )
			throw_runtime( "invalid HTTP line" );
		return std::move( result );
	}
}

namespace web
{

////////////////////////////////////////

response::response( net::tcp_socket &socket )
{
	std::string line = read_line( socket );
	std::cout << "STATUS: " << line << std::endl;
	while ( !line.empty() )
	{
		line = read_line( socket );
		if ( !line.empty() )
			std::cout << "HEADER: " << line << std::endl;
	}
}

////////////////////////////////////////

}

