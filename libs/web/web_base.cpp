//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "web_base.h"

namespace web
{

////////////////////////////////////////

web_base::web_base( void )
{
}

////////////////////////////////////////

web_base::web_base( std::string v )
	: _version( std::move( v ) )
{
}

////////////////////////////////////////

web_base::~web_base( void )
{
}

////////////////////////////////////////

std::string web_base::read_line( net::tcp_socket &socket )
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
	return result;
}

////////////////////////////////////////

void web_base::read_content( net::tcp_socket &socket )
{
	auto te = _header.find( "Transfer-Encoding" );
	auto cl = _header.find( "Content-Length" );
	if ( te != _header.end() )
	{
		if ( te->second == "chunked" )
		{
			size_t size = 0;
			do
			{
				std::string line = read_line( socket );
				size_t pos = 0;
				size = std::stoul( line, &pos, 16 );
				if ( size > 0 )
				{
					size_t off = _content.size();
					_content.resize( _content.size() + size );
					socket.read( &_content[off], size );
					char c[2];
					socket.read( &c, 2 );
					if ( c[0] != '\r' || c[1] != '\n' )
						throw_runtime( "invalid HTTP chunk" );
				}
			} while ( size > 0 );

		}
		else
			throw_runtime( "unknown Transfer-Encoding: {0}", te->second );
		/// @todo Read trailer headers
	}
	else if ( cl != _header.end() )
	{
		size_t size = std::stoul( cl->second, nullptr, 10 );
		size_t off = _content.size();
		_content.resize( _content.size() + size );
		socket.read( &_content[off], size );
	}
}

////////////////////////////////////////

}

