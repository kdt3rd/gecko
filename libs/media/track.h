
#pragma once

#include "frame.h"

#include <string>
#include <functional>
#include <memory>

namespace media
{

////////////////////////////////////////

class track
{
public:
	track( std::string n, int64_t b, int64_t e )
		: _name( n ), _begin( b ), _end( e )
	{
	}

	virtual ~track( void )
	{
	}

	const std::string &name( void ) const
	{
		return _name;
	}

	int64_t begin( void ) const
	{
		return _begin;
	}

	int64_t end( void ) const
	{
		return _end;
	}

private:
	std::string _name;
	int64_t _begin, _end;
};

////////////////////////////////////////

}

