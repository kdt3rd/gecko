
#pragma once

#include <vector>
#include "color.h"

namespace draw
{

////////////////////////////////////////

class gradient
{
public:
	gradient( void ) {}
	gradient( const gradient &g )
		: _stops( g._stops )
	{
	}

	gradient( std::initializer_list<std::pair<double,color>> l )
		: _stops( l )
	{
	}

	~gradient( void ) {}

	void add_stop( double v, const color &c ) { _stops.emplace_back( v, c ); }

	const std::vector<std::pair<double,color>> &stops( void ) const { return _stops; }

	gradient &operator=( const gradient &g ) { _stops = g._stops; return *this; }

private:
	gradient( gradient && ) = delete;
	std::vector<std::pair<double,color>> _stops;
};

////////////////////////////////////////

}

