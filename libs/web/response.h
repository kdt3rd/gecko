// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <net/tcp_socket.h>

#include "web_base.h"
#include "status_code.h"

namespace web
{

////////////////////////////////////////

/// @brief HTTP response
class response : public web_base
{
public:
	/// @brief Constructor
	response( void )
	{
	}

	/// @brief Constructor with content
	response( std::string &&c )
	{
		_content = std::move( c );
	}

	/// @brief Constructor from a server socket
	response( net::tcp_socket &socket );

	virtual ~response( void );
	response( const response & ) = default;
	response( response && ) = default;
	response &operator=( const response & ) = default;
	response &operator=( response && ) = default;

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

	/// @brief Send the response over the socket
	void send( net::tcp_socket &socket );

	/// @brief Send the response over the socket with content from a stream
	void send( net::tcp_socket &socket, std::istream &out );

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

private:
	status_code _status = status_code::OK;
	std::string _reason = reason_phrase( status_code::OK );
};

////////////////////////////////////////

/// @brief Debug print of response.
std::ostream &operator<<( std::ostream &out, const response &r );

////////////////////////////////////////

}

