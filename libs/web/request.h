
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

	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
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

}

