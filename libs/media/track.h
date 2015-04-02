
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
	track( std::string n, int64_t b, int64_t e, const std::function<std::shared_ptr<frame>(int64_t)> &get );
	~track( void );

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

	std::shared_ptr<frame> operator[]( int64_t f )
	{
		return _get( f );
	}

	std::shared_ptr<frame> at( int64_t f )
	{
		return _get( f );
	}

private:
	int64_t _begin, _end;
	std::function<std::shared_ptr<frame>(int64_t)> _get;
	std::string _name;
};

////////////////////////////////////////

}

