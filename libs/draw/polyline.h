
#pragma once

#include <vector>
#include <iostream>
#include <iterator>
#include <base/point.h>
#include <base/format.h>

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

	void save_svg_polyline( std::ostream &out, float width = 1.0, const std::string &color = "black" )
	{
		if ( _closed )
			out << "<polygon points = \"";
		else
			out << "<polyline points = \"";
		std::copy( begin(), end(), std::ostream_iterator<base::point>( out, " " ) );
		out << base::format( "\" stroke=\"{0}\" stroke-width=\"{1}\" fill=\"{2}\" />", color, width, "none" );
	}

	void save_svg_point_numbers( std::ostream &out, float width = 1.0, const std::string &color = "black" )
	{
		for ( size_t i = 0; i < _points.size(); ++i )
		{
			const auto &p = _points[i];
			out << base::format( "<text x=\"{0}\" y=\"{1}\" fill=\"#00f\">{2}</text>", p.x(), p.y(), i );
		}
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

