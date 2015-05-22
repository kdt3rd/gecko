
#pragma once

#include <base/uri.h>
#include <net/tcp_socket.h>
#include <map>

namespace web
{

////////////////////////////////////////

class request
{
public:
	request( net::tcp_socket &socket );
	request( std::string method, const base::uri &path, std::string version = "1.1" );

	const std::string &method( void ) const
	{
		return _method;
	}

	const base::uri &path( void ) const
	{
		return _path;
	}

	const std::string &version( void ) const
	{
		return _version;
	}

	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
	}

	std::string operator[]( const std::string &n )
	{
		return _header.at( n );
	}

	const std::map<std::string,std::string> &header( void ) const
	{
		return _header;
	}

	void send( net::tcp_socket &socket );

private:
	std::string _method;
	base::uri _path;
	std::string _version;
	std::map<std::string,std::string> _header;
	std::stringstream _content;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const request &r );

////////////////////////////////////////

}

