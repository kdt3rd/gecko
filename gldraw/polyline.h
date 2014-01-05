
#pragma once

#include <vector>
#include "point.h"
#include "polyline.h"

namespace gldraw
{

////////////////////////////////////////

class polyline
{
public:
	void close( void )
	{
		_closed = true;
		_points.emplace_back( _points.front() );
	}

	bool closed( void ) const
	{
		return _closed;
	}

	bool empty( void ) const
	{
		return _points.empty();
	}

	const point &back( void ) const
	{
		return _points.back();
	}

	size_t size( void ) const
	{
		return _points.size();
	}

	void push_back( const point &p )
	{
		_points.push_back( p );
	}

	std::vector<point>::iterator begin( void )
	{
		return _points.begin();
	}

	std::vector<point>::const_iterator begin( void ) const
	{
		return _points.begin();
	}

	std::vector<point>::iterator end( void )
	{
		return _points.end();
	}

	std::vector<point>::const_iterator end( void ) const
	{
		return _points.end();
	}

private:
	bool _closed = false;
	std::vector<point> _points;
};

////////////////////////////////////////

}

