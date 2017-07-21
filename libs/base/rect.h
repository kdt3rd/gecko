//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
	/// @brief Default constructor
	rect( void ) = default;

	/// @brief Constructor from point, width, and height
	rect( const point &p, double w, double h )
		: _position( p ), _extent( w, h )
	{
		fix_extent();
	}

	/// @brief Constructor from point and size
	rect( const point &p, const size &s )
		: _position( p ), _extent( s )
	{
		fix_extent();
	}

	/// @brief Constructor from width and height
	explicit rect( double w, double h )
		: _extent( w, h )
	{
		fix_extent();
	}

	rect( double x, double y, double w, double h )
		: _position( x, y ), _extent( w, h )
	{
		fix_extent();
	}

	/// @brief Constructor from size
	rect( const size &s )
		: _extent( s )
	{
		fix_extent();
	}

	/// @brief X coordinate
	double x( void ) const { return _position.x(); }

	/// @brief Y coordinate
	double y( void ) const { return _position.y(); }

	/// @brief Width
	double width( void ) const { return _extent.w(); }

	/// @brief Height
	double height( void ) const { return _extent.h(); }

	/// @brief X coordinate in the rectangle
	double x( double percent, double radius = 0.0 ) const { return _position.x() + radius + ( _extent.w() - radius*2.0 ) * percent; }

	/// @brief Y coordinate in the rectangle
	double y( double percent, double radius = 0.0 ) const { return _position.y() + radius + ( _extent.h() - radius*2.0 ) * percent; }

	/// @brief X coordinate of right side
	double x1( void ) const { return _position.x(); }

	/// @brief Y coordinate of top side
	double y1( void ) const { return _position.y(); }

	/// @brief X coordinate of right side
	double x2( void ) const { return _position.x() + _extent.w() - 1.0; }

	/// @brief Y coordinate of bottom side
	double y2( void ) const { return _position.y() + _extent.h() - 1.0; }

	/// @brief Position of the rectangle
	const point &position( void ) const { return _position; }

	/// @brief Size of the rectangle
	const size &extent( void ) const { return _extent; }

	/// @brief Top left corner
	point top_left( void ) const { return _position; }

	/// @brief Top right corner
	point top_right( void ) const { return point( x2(), y1() ); }

	/// @brief Top center point
	point top_center( void ) const { return point( ( x1() + x2() ) / 2.0, y1() ); }

	/// @brief Bottom left corner
	point bottom_left( void ) const { return point( x1(), y2() ); }

	/// @brief Bottom right corner
	point bottom_right( void ) const { return point( x2(), y2() ); }

	/// @brief Move the rectangle
	void set_center( const point &p )
	{
		_position = _position + ( p - center() );
	}

	/// @brief Center of rectangle
	point center( void ) const { return point( ( x1() + x2() ) / 2.0, ( y1() + y2() ) / 2.0 ); }

	/// @brief Radius of rectangle
	double radius( void ) const { return std::min( width(), height() ) / 2.0; }

	/// @brief Set position and size
	void set( const point &p, const size &s )
	{
		_position = p;
		_extent = s;
		fix_extent();
	}

	/// @brief Set x position
	void set_x( double x ) { _position.set_x( x ); }

	/// @brief Set y position
	void set_y( double y ) { _position.set_y( y ); }

	/// @brief Set width
	void set_width( double w ) { _extent.set_width( w ); fix_extent(); }

	/// @brief Set height
	void set_height( double h ) { _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( double w, double h ) { _extent.set_width( w ); _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( const base::size &s ) { _extent = s; }

	/// @brief Set left side
	void set_x1( double x ) { _position.set_x( x ); }

	/// @brief Set top side
	void set_y1( double y ) { _position.set_y( y ); }

	/// @brief Set right side
	void set_x2( double x ) { _extent.set_width( x - x1() ); }

	/// @brief Set bottom side
	void set_y2( double y ) { _extent.set_height( y - y1() ); }

	/// @brief Set left and right sides
	void set_horizontal( double x1, double x2 );

	/// @brief Set top and bottom sides
	void set_vertical( double y1, double y2 );

	/// @brief Set position
	void set_position( const point &p ) { _position = p; }

	/// @brief Set size
	void set_extent( const size &s ) { _extent = s; fix_extent(); }

	/// @brief Move rectangle by x, y
	void move_by( double x, double y ) { _position.move_by( x, y ); }

	/// @brief Set size
	void set_extent( double w, double h ) { _extent.set( w, h ); }

	/// @brief Trim rectangle on all sides
	void trim( double l, double r, double t, double b ) { _extent.shrink( l + r, t + b ); _position.move_by( l, t ); }

	/// @brief Is x,y contained in the rectangle?
	bool contains( double x, double y ) const;

	/// @brief Is the point contained in the rectangle?
	bool contains( const point &p ) const { return contains( p.x(), p.y() ); }

	/// @brief Shrink the rectangle
	void shrink( double left, double right, double top, double bottom );

	/// @brief Grow the rectangle
	void grow( double left, double right, double top, double bottom );

	void include( rect &other );

	bool empty( void ) const
	{
		return _extent.empty();
	}

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

/// @brief Output operator for rectangle.
inline std::ostream &operator<<( std::ostream &out, const rect &r )
{
	out << r.position() << ' ' << r.extent();
	return out;
}

////////////////////////////////////////

} // namespace base

