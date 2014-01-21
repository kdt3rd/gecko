
#pragma once

#include <vector>
#include <memory>
#include <core/color.h>

namespace draw
{

////////////////////////////////////////

/// @brief Gradient.
class gradient
{
public:
	gradient( void )
	{
	}

	gradient( const gradient &g )
		: _stops( g._stops )
	{
	}

	gradient( std::initializer_list<std::pair<double,core::color>> l )
		: _stops( l )
	{
	}

	~gradient( void ) {}

	void add_stop( double v, const core::color &c ) { _stops.emplace_back( v, c ); }

	core::color sample( double v ) const;

	const std::vector<std::pair<double,core::color>> &stops( void ) const { return _stops; }

	gradient &operator=( const gradient &g ) { _stops = g._stops; return *this; }

private:
	gradient( gradient && ) = delete;

	std::vector<std::pair<double,core::color>> _stops;
};

////////////////////////////////////////

}

