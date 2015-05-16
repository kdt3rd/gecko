
#pragma once

#include <cstdint>

namespace net
{

////////////////////////////////////////

class socket
{
public:
	socket( void );
	socket( const socket &that ) = delete;
	~socket( void );

	socket &operator=( const socket &that ) = delete;

	void bind( uint16_t port );
	void bind( uint32_t host, uint16_t port );

protected:
	int _socket = -1;
};

////////////////////////////////////////

}

