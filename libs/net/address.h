
#pragma once

#include <string>
#include <netinet/ip.h>

namespace net
{

////////////////////////////////////////

class address
{
public:
	enum class special : uint32_t
	{
		NONE = INADDR_NONE,
		LOOPBACK = INADDR_LOOPBACK,
		ANY = INADDR_ANY,
		BROADCAST = INADDR_BROADCAST
	};

	address( const char *name )
	{
		lookup_name( name );
	}

	address( const std::string &name )
	{
		lookup_name( name.c_str() );
	}

	address( uint32_t addr )
		: _addr( addr )
	{
	}

	address( special addr )
		: _addr( static_cast<uint32_t>( addr ) )
	{
	}

	operator uint32_t( void ) const
	{
		return _addr;
	}

private:
	void lookup_name( const char *name );

	uint32_t _addr;
};

////////////////////////////////////////

}

