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
//template <typename T>
class rect
{
public:
//	typedef T coord_type;
	typedef double coord_type;
	//using point = point<coord_type>;
	/// @brief Default constructor
	rect( void ) = default;

	/// @brief Constructor from point, width, and height
	rect( const point &p, coord_type w, coord_type h )
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
	explicit rect( coord_type w, coord_type h )
		: _extent( w, h )
	{
		fix_extent();
	}

	rect( coord_type x, coord_type y, coord_type w, coord_type h )
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
	coord_type x( void ) const { return _position.x(); }

	/// @brief Y coordinate
	coord_type y( void ) const { return _position.y(); }

	/// @brief Width
	coord_type width( void ) const { return _extent.w(); }

	/// @brief Height
	coord_type height( void ) const { return _extent.h(); }

	/// @brief X coordinate in the rectangle
	coord_type x( coord_type percent, coord_type radius = coord_type(0) ) const { return _position.x() + radius + ( _extent.w() - radius*coord_type(2) ) * percent; }

	/// @brief Y coordinate in the rectangle
	coord_type y( coord_type percent, coord_type radius = coord_type(0) ) const { return _position.y() + radius + ( _extent.h() - radius*coord_type(2) ) * percent; }

	/// @brief X coordinate of right side
	coord_type x1( void ) const { return _position.x(); }

	/// @brief Y coordinate of top side
	coord_type y1( void ) const { return _position.y(); }

	/// @brief X coordinate of right side
	coord_type x2( void ) const { return _position.x() + _extent.w() - coord_type(1); }

	/// @brief Y coordinate of bottom side
	coord_type y2( void ) const { return _position.y() + _extent.h() - coord_type(1); }

	/// @brief Position of the rectangle
	const point &position( void ) const { return _position; }

	/// @brief Size of the rectangle
	const size &extent( void ) const { return _extent; }

	/// @brief Top left corner
	point top_left( void ) const { return _position; }

	/// @brief Top right corner
	point top_right( void ) const { return point( x2(), y1() ); }

	/// @brief Top center point
	point top_center( void ) const { return point( ( x1() + x2() ) / coord_type(2), y1() ); }

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
	point center( void ) const { return point( ( x1() + x2() ) / coord_type(2), ( y1() + y2() ) / coord_type(2) ); }

	/// @brief Radius of rectangle
	coord_type radius( void ) const { return std::min( width(), height() ) / coord_type(2); }

	/// @brief Set position and size
	void set( const point &p, const size &s )
	{
		_position = p;
		_extent = s;
		fix_extent();
	}

	/// @brief Set x position
	void set_x( coord_type x ) { _position.set_x( x ); }

	/// @brief Set y position
	void set_y( coord_type y ) { _position.set_y( y ); }

	/// @brief Set width
	void set_width( coord_type w ) { _extent.set_width( w ); fix_extent(); }

	/// @brief Set height
	void set_height( coord_type h ) { _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( coord_type w, coord_type h ) { _extent.set_width( w ); _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( const base::size &s ) { _extent = s; }

	/// @brief Set left side
	void set_x1( coord_type x ) { _position.set_x( x ); }

	/// @brief Set top side
	void set_y1( coord_type y ) { _position.set_y( y ); }

	/// @brief Set right side
	void set_x2( coord_type x ) { _extent.set_width( x - x1() ); }

	/// @brief Set bottom side
	void set_y2( coord_type y ) { _extent.set_height( y - y1() ); }

	/// @brief Set left and right sides
	void set_horizontal( coord_type x1, coord_type x2 );

	/// @brief Set top and bottom sides
	void set_vertical( coord_type y1, coord_type y2 );

	/// @brief Set position
	void set_position( const point &p ) { _position = p; }

	/// @brief Set size
	void set_extent( const size &s ) { _extent = s; fix_extent(); }

	/// @brief Move rectangle by x, y
	void move_by( coord_type x, coord_type y ) { _position.move_by( x, y ); }

	/// @brief Set size
	void set_extent( coord_type w, coord_type h ) { _extent.set( w, h ); }

	/// @brief Trim rectangle on all sides
	void trim( coord_type l, coord_type r, coord_type t, coord_type b ) { _extent.shrink( l + r, t + b ); _position.move_by( l, t ); }

	/// @brief Is x,y contained in the rectangle?
	bool contains( coord_type x, coord_type y ) const;

	/// @brief Is the point contained in the rectangle?
	bool contains( const point &p ) const { return contains( p.x(), p.y() ); }

	/// @brief Shrink the rectangle
	void shrink( coord_type left, coord_type right, coord_type top, coord_type bottom );

	/// @brief Grow the rectangle
	void grow( coord_type left, coord_type right, coord_type top, coord_type bottom );

	void include( const rect &other );

	double distance( const rect &other ) const;

	void clip( const rect &other );

	bool empty( void ) const
	{
		return _extent.empty();
	}

private:
	void fix_extent( void )
	{
		_position.move_by( std::min( _extent.w(), coord_type(0) ), std::min( _extent.h(), coord_type(0) ) );
		_extent.set( std::abs( _extent.w() ), std::abs( _extent.h() ) );
	}

	point _position;
	size _extent;
};

////////////////////////////////////////

/// @brief Output operator for rectangle.
//template <typename T>
//inline std::ostream &operator<<( std::ostream &out, const rect<T> &r )
inline std::ostream &operator<<( std::ostream &out, const rect &r )
{
	out << r.position() << ' ' << r.extent();
	return out;
}

////////////////////////////////////////

} // namespace base

