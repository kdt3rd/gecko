
#pragma once

#include <sstream>
#include <map>
#include <string>
#include <net/tcp_socket.h>

namespace web
{

////////////////////////////////////////

class response
{
public:
	response( void )
	{
	}

	response( net::tcp_socket &socket );

	response &operator<<( const std::string &t )
	{
		_content += t;
		return *this;
	}

	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
	}

	void send( net::tcp_socket &socket );

private:
	std::string _content;
	std::string _version = "1.1";
	size_t _status = 200;
	std::map<std::string,std::string> _header;
};

////////////////////////////////////////


}

