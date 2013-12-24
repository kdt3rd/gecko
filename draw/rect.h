
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
		fix_size();
	}

	rect( double w, double h )
		: _w( w ), _h( h )
	{
		fix_size();
	}

	double x( void ) const { return _position.x(); }
	double y( void ) const { return _position.y(); }
	double width( void ) const { return _w; }
	double height( void ) const { return _h; }

	double x( double percent, double radius = 0.0 ) const { return _position.x() + radius + ( _w - radius*2.0 ) * percent; }
	double y( double percent, double radius = 0.0 ) const { return _position.y() + radius + ( _h - radius*2.0 ) * percent; }

	double x1( void ) const { return _position.x(); }
	double y1( void ) const { return _position.y(); }
	double x2( void ) const { return _position.x() + _w - 1.0; }
	double y2( void ) const { return _position.y() + _h - 1.0; }

	point position( void ) const { return _position; }
	point top_left( void ) const { return _position; }
	point top_right( void ) const { return point( x2(), y1() ); }
	point bottom_left( void ) const { return point( x1(), y2() ); }
	point bottom_right( void ) const { return point( x2(), y2() ); }

	point center( void ) const { return point( ( x1() + x2() ) / 2.0, ( y1() + y2() ) / 2.0 ); }
	double radius( void ) const { return std::min( width(), height() ) / 2.0; }

	void set_x( double x ) { _position.set_x( x ); }
	void set_y( double y ) { _position.set_y( y ); }
	void set_width( double w ) { _w = w; fix_size(); }
	void set_height( double h ) { _h = h; fix_size(); }

	void set_horizontal( double x1, double x2 );
	void set_vertical( double y1, double y2 );

	void set_position( const draw::point &p ) { _position = p; }
	void move_by( double x, double y ) { _position.move_by( x, y ); }

	void set_size( double w, double h ) { _w = w; _h = h; fix_size(); }

	void trim( double l, double r, double t, double b ) { _w -= l + r; _h -= t + b; _position.move_by( l, t ); }

	bool contains( double x, double y ) const;
	bool contains( const draw::point &p ) const { return contains( p.x(), p.y() ); }

	void shrink( double left, double right, double top, double bottom );

private:
	void fix_size( void )
	{
		if ( _w < 0.0 )
		{
			_position.move_by( _w, 0.0 );
			_w = -_w;
		}

		if ( _h < 0.0 )
		{
			_position.move_by( 0.0, _h );
			_h = -_h;
		}
	}

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

