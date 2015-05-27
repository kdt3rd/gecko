
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

/// @brief HTTP response
class response
{
public:
	/// @brief Constructor
	response( void )
	{
	}

	/// @brief Constructor with content
	response( std::string &&c )
		: _content( std::move( c ) )
	{
	}

	/// @brief Constructor from a server socket
	response( net::tcp_socket &socket );

	/// @brief Stream operator to add to response content.
	response &operator<<( const std::string &t )
	{
		_content += t;
		return *this;
	}

	/// @brief Set HTTP status code (defaults to 200 Okay)
	void set_status_code( status_code c )
	{
		_status = c;
		_reason = reason_phrase( _status );
	}

	/// @brief Set HTTP status code with custom reason
	void set_status_code( status_code c, std::string reason )
	{
		_status = c;
		_reason = std::move( reason );
	}

	/// @brief Set an HTTP header value.
	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
	}

	/// @brief Send the response over the socket
	void send( net::tcp_socket &socket );

	/// @brief Send the response over the socket with content from a stream
	void send( net::tcp_socket &socket, std::istream &out );

	/// @brief The response content
	const std::string &content( void ) const
	{
		return _content;
	}

	/// @brief The HTTP version (default to "1.1")
	const std::string &version( void ) const
	{
		return _version;
	}

	/// @brief The HTTP status code.
	status_code status( void ) const
	{
		return _status;
	}

	/// @brief The HTTP reason.
	const std::string &reason( void ) const
	{
		return _reason;
	}

	/// @brief The map of HTTP header values.
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

/// @brief Debug print of response.
std::ostream &operator<<( std::ostream &out, const response &r );

////////////////////////////////////////

}

