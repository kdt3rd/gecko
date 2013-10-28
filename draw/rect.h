
#pragma once

#include "point.h"
#include <cmath>

namespace draw
{

////////////////////////////////////////

class rect
{
public:
	rect( void )
	{
	}

	rect( const point &p, double ww, double hh )
		: _position( p ), _w( ww ), _h( hh )
	{
	}

	double x( void ) const { return _position.x(); }
	double y( void ) const { return _position.y(); }
	double x1( void ) const { return _position.x(); }
	double y1( void ) const { return _position.y(); }
	double x2( void ) const { return _position.x() + _w - 1.0; }
	double y2( void ) const { return _position.y() + _h - 1.0; }

	point top_left( void ) const { return _position; }
	point top_right( void ) const { return point( x2(), y1() ); }
	point bottom_left( void ) const { return point( x1(), y2() ); }
	point bottom_right( void ) const { return point( x2(), y2() ); }

	double width( void ) const { return _w; }
	double height( void ) const { return _h; }

	void set_x( double x ) { _position.set_x( x ); }
	void set_y( double y ) { _position.set_y( y ); }
	void set_width( double w ) { _w = w; }
	void set_height( double h ) { _h = h; }

private:
	point _position;
	double _w = 0.0, _h = 0.0;
};

////////////////////////////////////////

}

