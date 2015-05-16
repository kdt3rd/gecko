
#pragma once

#include <sstream>
#include <net/tcp_socket.h>

namespace web
{

////////////////////////////////////////

class response
{
public:
	response( net::tcp_socket &socket );

	response( response &&r )
		: _content( r._content.str() )
	{
	}

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

private:
	std::stringstream _content;
};

////////////////////////////////////////


}

