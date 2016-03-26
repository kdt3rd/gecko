
#pragma once

#include <vector>
#include <memory>
#include <gl/color.h>

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

	gradient( std::initializer_list<std::pair<float,gl::color>> l )
		: _stops( l )
	{
	}

	~gradient( void ) {}

	void add_stop( float v, const gl::color &c ) { _stops.emplace_back( v, c ); }

	void sort( void );

	gl::color sample( float v ) const;

	const std::vector<std::pair<float,gl::color>> &stops( void ) const { return _stops; }

	gradient &operator=( const gradient &g ) { _stops = g._stops; return *this; }

	void clear( void ) { _stops.clear(); }

private:
	gradient( gradient && ) = delete;

	std::vector<std::pair<float,gl::color>> _stops;
};

////////////////////////////////////////

}

