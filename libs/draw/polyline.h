
#pragma once

#include <vector>
#include <iostream>
#include <iterator>
#include <base/point.h>

namespace draw
{

////////////////////////////////////////

class polyline
{
public:
	using point = base::point;

	void close( void )
	{
		_closed = true;
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

	const point &front( void ) const
	{
		return _points.front();
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

	const point &operator[]( size_t i ) const
	{
		return _points[i];
	}

private:
	bool _closed = false;
	std::vector<point> _points;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const polyline &p )
{
	std::copy( p.begin(), p.end(), std::ostream_iterator<base::point>( out, "; " ) );
	return out;
}

////////////////////////////////////////

}
