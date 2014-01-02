
#pragma once

#include "point.h"
#include "size.h"
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
		: _position( p ), _size( w, h )
	{
		fix_size();
	}

	rect( const point &p, const draw::size &s )
		: _position( p ), _size( s )
	{
		fix_size();
	}

	rect( double w, double h )
		: _size( w, h )
	{
		fix_size();
	}

	rect( const draw::size &s )
		: _size( s )
	{
		fix_size();
	}

	double x( void ) const { return _position.x(); }
	double y( void ) const { return _position.y(); }
	double width( void ) const { return _size.w(); }
	double height( void ) const { return _size.h(); }

	double x( double percent, double radius = 0.0 ) const { return _position.x() + radius + ( _size.w() - radius*2.0 ) * percent; }
	double y( double percent, double radius = 0.0 ) const { return _position.y() + radius + ( _size.h() - radius*2.0 ) * percent; }

	double x1( void ) const { return _position.x(); }
	double y1( void ) const { return _position.y(); }
	double x2( void ) const { return _position.x() + _size.w() - 1.0; }
	double y2( void ) const { return _position.y() + _size.h() - 1.0; }

	point position( void ) const { return _position; }
	draw::size size( void ) const { return _size; }
	point top_left( void ) const { return _position; }
	point top_right( void ) const { return point( x2(), y1() ); }
	point bottom_left( void ) const { return point( x1(), y2() ); }
	point bottom_right( void ) const { return point( x2(), y2() ); }

	point center( void ) const { return point( ( x1() + x2() ) / 2.0, ( y1() + y2() ) / 2.0 ); }
	double radius( void ) const { return std::min( width(), height() ) / 2.0; }

	void set_x( double x ) { _position.set_x( x ); }
	void set_y( double y ) { _position.set_y( y ); }
	void set_width( double w ) { _size.set_width( w ); fix_size(); }
	void set_height( double h ) { _size.set_height( h ); fix_size(); }

	void set_horizontal( double x1, double x2 );
	void set_vertical( double y1, double y2 );

	void set_position( const draw::point &p ) { _position = p; }
	void set_size( const draw::size &s ) { _size = s; fix_size(); }
	void move_by( double x, double y ) { _position.move_by( x, y ); }

	void set_size( double w, double h ) { _size.set( w, h ); }

	void trim( double l, double r, double t, double b ) { _size.shrink( l + r, t + b ); _position.move_by( l, t ); }

	bool contains( double x, double y ) const;
	bool contains( const draw::point &p ) const { return contains( p.x(), p.y() ); }

	void shrink( double left, double right, double top, double bottom );
	void grow( double left, double right, double top, double bottom );

private:
	void fix_size( void )
	{
		_position.move_by( std::min( _size.w(), 0.0 ), std::min( _size.h(), 0.0 ) );
		_size.set( std::abs( _size.w() ), std::abs( _size.h() ) );
	}

	point _position;
	draw::size _size;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const rect &r )
{
	out << r.position() << ' ' << r.size();
	return out;
}

////////////////////////////////////////

}

