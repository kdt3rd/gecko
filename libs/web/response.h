
#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <net/tcp_socket.h>

#include "status_code.h"

namespace web
{

////////////////////////////////////////

class response
{
public:
	response( void )
	{
	}

	response( std::string &&c )
		: _content( std::move( c ) )
	{
	}

	response( net::tcp_socket &socket );

	response &operator<<( const std::string &t )
	{
		_content += t;
		return *this;
	}

	void set_status_code( status_code c )
	{
		_status = c;
		_reason = reason_phrase( _status );
	}

	void set_status_code( status_code c, std::string reason )
	{
		_status = c;
		_reason = std::move( reason );
	}

	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
	}

	void send( net::tcp_socket &socket );

	void send( net::tcp_socket &socket, std::istream &out );

	const std::string &content( void ) const
	{
		return _content;
	}

	const std::string &version( void ) const
	{
		return _version;
	}

	status_code status( void ) const
	{
		return _status;
	}

	const std::string &reason( void ) const
	{
		return _reason;
	}

	const std::map<std::string,std::string> &header( void ) const
	{
		return _header;
	}

private:
	std::string _content;
	std::string _version = "1.1";
	status_code _status = status_code::OK;
	std::string _reason = reason_phrase( status_code::OK );
	std::map<std::string,std::string> _header;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const response &r );

////////////////////////////////////////

}

