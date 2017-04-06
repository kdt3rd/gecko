//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>

namespace net
{

////////////////////////////////////////

class socket
{
public:
	socket( void );
	socket( socket &&that )
		: _socket( that._socket )
	{
		that._socket = -1;
	}

	socket( const socket &that ) = delete;
	~socket( void );

	socket &operator=( const socket &that ) = delete;

	void bind( uint16_t port );
	void bind( uint32_t host, uint16_t port );

	explicit operator bool( void ) const
	{
		return _socket > -1;
	}

	int get_socket( void ) const
	{
		return _socket;
	}

	void close( void );

protected:
	int _socket = -1;
};

////////////////////////////////////////

}

