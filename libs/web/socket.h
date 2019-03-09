// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "request.h"
#include <base/signal.h>
#include <base/json.h>
#include <random>

namespace web
{

////////////////////////////////////////

/// @brief WebSocket class.
class socket
{
public:
	/// @brief Open a WebSocket from a request
	socket( request &req, net::tcp_socket &&client );

	/// @brief Open a WebSocket to a server
	socket( base::uri &server, const std::string &agent = "WSClient/0.0", double timeout = 0.0 );

	/// @brief Destructor
	~socket( void );

	/// @brief Message callback
	base::signal<void(const std::string &,bool)> when_message;

	/// @brief Send a message.
	void send( const char *msg, size_t len );

	/// @brief Send a message.
	void send( const char *msg )
	{
		send( msg, std::strlen( msg ) );
	}

	/// @brief Send a message.
	void send( const std::string &msg )
	{
		send( msg.c_str(), msg.size() );
	}

	/// @brief Send a message.
	void send( const base::json &msg );

	/// @brief Run the socket.
	void run( void );

	/// @brief Wait for a message.
	/// @param message String to fill in with message.
	/// @param binary Set to true of the message is binary, false if text.
	/// @returns false if the connection closed.
	bool wait( std::string &message, bool &binary );

	/// @brief Wait for a message.
	/// @param message String to fill in with message.
	/// @returns false if the connection closed.
	bool wait( std::string &message )
	{
		bool bin;
		return wait( message, bin );
	}


	/// @brief Close the socket.
	void close( void );

protected:
	uint8_t random_byte( void )
	{
		return _dist8( _rand );
	}

	bool _masked = false;
	bool _closed = false;
	net::tcp_socket _socket;
	std::random_device _rand;
	std::uniform_int_distribution<uint8_t> _dist8;
};

////////////////////////////////////////

}

