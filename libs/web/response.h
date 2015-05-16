
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
	response( net::tcp_socket &socket );

	/*
	template <class T>
	response &operator<<( const T &t )
	{
		_content << t;
		return *this;
	}

	response &operator<<( std::ostream &(*manip)( std::ostream & ) )
	{
		_content << manip;
		return *this;
	}

	response &operator<<( response &(*manip)( response & ) )
	{
		return manip( *this );
	}
	*/

private:
	std::string _content;
	std::map<std::string,std::string> _header;
};

////////////////////////////////////////


}

