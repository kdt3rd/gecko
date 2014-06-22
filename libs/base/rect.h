
#pragma once

#include "point.h"
#include "size.h"
#include <algorithm>
#include <iostream>
#include <cmath>

namespace base
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
		: _position( p ), _extent( w, h )
	{
		fix_extent();
	}

	rect( const point &p, const size &s )
		: _position( p ), _extent( s )
	{
		fix_extent();
	}

	rect( double w, double h )
		: _extent( w, h )
	{
		fix_extent();
	}

	rect( const size &s )
		: _extent( s )
	{
		fix_extent();
	}

	double x( void ) const { return _position.x(); }
	double y( void ) const { return _position.y(); }
	double width( void ) const { return _extent.w(); }
	double height( void ) const { return _extent.h(); }

	double x( double percent, double radius = 0.0 ) const { return _position.x() + radius + ( _extent.w() - radius*2.0 ) * percent; }
	double y( double percent, double radius = 0.0 ) const { return _position.y() + radius + ( _extent.h() - radius*2.0 ) * percent; }

	double x1( void ) const { return _position.x(); }
	double y1( void ) const { return _position.y(); }
	double x2( void ) const { return _position.x() + _extent.w() - 1.0; }
	double y2( void ) const { return _position.y() + _extent.h() - 1.0; }

	point position( void ) const { return _position; }
	size extent( void ) const { return _extent; }
	point top_left( void ) const { return _position; }
	point top_right( void ) const { return point( x2(), y1() ); }
	point bottom_left( void ) const { return point( x1(), y2() ); }
	point bottom_right( void ) const { return point( x2(), y2() ); }

	void set_center( const point &p )
	{
		_position = _position + ( p - center() );
	}

	point center( void ) const { return point( ( x1() + x2() ) / 2.0, ( y1() + y2() ) / 2.0 ); }
	double radius( void ) const { return std::min( width(), height() ) / 2.0; }

	void set( const point &p, const size &s )
	{
		_position = p;
		_extent = s;
		fix_extent();
	}

	void set_x( double x ) { _position.set_x( x ); }
	void set_y( double y ) { _position.set_y( y ); }
	void set_width( double w ) { _extent.set_width( w ); fix_extent(); }
	void set_height( double h ) { _extent.set_height( h ); fix_extent(); }

	void set_x1( double x ) { _position.set_x( x ); }
	void set_y1( double y ) { _position.set_y( y ); }
	void set_x2( double x ) { _extent.set_width( x - x1() ); }
	void set_y2( double y ) { _extent.set_height( y - y1() ); }

	void set_horizontal( double x1, double x2 );
	void set_vertical( double y1, double y2 );

	void set_position( const point &p ) { _position = p; }
	void set_extent( const size &s ) { _extent = s; fix_extent(); }
	void move_by( double x, double y ) { _position.move_by( x, y ); }

	void set_extent( double w, double h ) { _extent.set( w, h ); }

	void trim( double l, double r, double t, double b ) { _extent.shrink( l + r, t + b ); _position.move_by( l, t ); }

	bool contains( double x, double y ) const;
	bool contains( const point &p ) const { return contains( p.x(), p.y() ); }

	void shrink( double left, double right, double top, double bottom );
	void grow( double left, double right, double top, double bottom );

private:
	void fix_extent( void )
	{
		_position.move_by( std::min( _extent.w(), 0.0 ), std::min( _extent.h(), 0.0 ) );
		_extent.set( std::abs( _extent.w() ), std::abs( _extent.h() ) );
	}

	point _position;
	size _extent;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const rect &r )
{
	out << r.position() << ' ' << r.extent();
	return out;
}

////////////////////////////////////////

}

