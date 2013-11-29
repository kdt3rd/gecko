
#pragma once

#include "point.h"
#include <algorithm>
#include <iostream>
#include <cmath>

namespace draw
{

////////////////////////////////////////

/// @brief Rectangle.
///
/// Thing with parallel sides, with opposing sides being equal in length.
class rect
{
public:
	rect( void )
	{
	}

	rect( const point &p, double w, double h )
		: _position( p ), _w( w ), _h( h )
	{
	}

	rect( double w, double h )
		: _w( w ), _h( h )
	{
	}

	double x( void ) const { return _position.x(); }
	double y( void ) const { return _position.y(); }
	double width( void ) const { return _w; }
	double height( void ) const { return _h; }

	double x1( void ) const { return _position.x(); }
	double y1( void ) const { return _position.y(); }
	double x2( void ) const { return _position.x() + _w - 1.0; }
	double y2( void ) const { return _position.y() + _h - 1.0; }

	point top_left( void ) const { return _position; }
	point top_right( void ) const { return point( x2(), y1() ); }
	point bottom_left( void ) const { return point( x1(), y2() ); }
	point bottom_right( void ) const { return point( x2(), y2() ); }

	point center( void ) const { return point( ( x1() + x2() ) / 2.0, ( y1() + y2() ) / 2.0 ); }
	double radius( void ) const { return std::min( width(), height() ) / 2.0; }

	void set_x( double x ) { _position.set_x( x ); }
	void set_y( double y ) { _position.set_y( y ); }
	void set_width( double w ) { _w = w; }
	void set_height( double h ) { _h = h; }

	void set_horizontal( double x1, double x2 );
	void set_vertical( double y1, double y2 );

	void set_size( double w, double h ) { _w = w; _h = h; }

	bool contains( double x, double y ) const;

private:
	point _position;
	double _w = 0.0, _h = 0.0;
};

inline std::ostream &operator<<( std::ostream &out, const rect &r )
{
	out << r.x() << ',' << r.y() << ' ' << r.width() << 'x' << r.height();
	return out;
}

////////////////////////////////////////

}

