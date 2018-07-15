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
template <typename T>
class rect
{
public:
	using coord_type = T;
	using point_type = point<coord_type, 2>;
	using size_type = size<coord_type>;

	static constexpr coord_type kEndOff = std::is_integral<T>::value ? coord_type(1) : coord_type(0);

	constexpr rect( void ) = default;
	constexpr rect( const rect & ) = default;
	constexpr rect( rect && ) noexcept = default;
	rect &operator=( const rect & ) = default;
	rect &operator=( rect && ) noexcept = default;
	~rect( void ) = default;

	/// @brief Constructor from point, width, and height
	rect( const point_type &p, coord_type w, coord_type h )
		: _position( p ), _extent( w, h )
	{
		fix_extent();
	}

	/// @brief Constructor from point and size
	rect( const point_type &p, const size_type &s )
		: _position( p ), _extent( s )
	{
		fix_extent();
	}

	/// @brief Constructor from width and height
	rect( coord_type w, coord_type h )
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
	rect( const size_type &s )
		: _extent( s )
	{
		fix_extent();
	}

	/// @brief allow explicit construction from other rectangle type
	template <typename U>
	explicit constexpr rect( const rect<U> &o )
		: _position( point_type( o.position() ) ), _extent( size_type( o.extent() ) )
	{}

	/// @brief X coordinate
	constexpr coord_type x( void ) const { return _position[0]; }

	/// @brief Y coordinate
	constexpr coord_type y( void ) const { return _position[1]; }

	/// @brief Width
	constexpr coord_type width( void ) const { return _extent.w(); }

	/// @brief Height
	constexpr coord_type height( void ) const { return _extent.h(); }

	/// @brief X coordinate in the rectangle
	constexpr coord_type x( coord_type percent, coord_type radius = coord_type(0) ) const
	{
		static_assert( !std::is_integral<coord_type>::value, "expect a floating point type for percent accessor" );
		return _position[0] + radius + ( _extent.w() - radius*coord_type(2) ) * percent;
	}

	/// @brief Y coordinate in the rectangle
	constexpr coord_type y( coord_type percent, coord_type radius = coord_type(0) ) const
	{
		static_assert( !std::is_integral<coord_type>::value, "expect a floating point type for percent accessor" );
		return _position[1] + radius + ( _extent.h() - radius*coord_type(2) ) * percent;
	}

	/// @brief X coordinate of right side
	constexpr coord_type x1( void ) const { return _position[0]; }

	/// @brief Y coordinate of top side
	constexpr coord_type y1( void ) const { return _position[1]; }

	/// @brief X coordinate of right side
	constexpr coord_type x2( void ) const { return _position[0] + _extent.w() - kEndOff; }

	/// @brief Y coordinate of bottom side
	constexpr coord_type y2( void ) const { return _position[1] + _extent.h() - kEndOff; }

	/// @brief Position of the rectangle
	constexpr const point_type &position( void ) const { return _position; }

	/// @brief Size of the rectangle
	constexpr const size_type &extent( void ) const { return _extent; }

	/// @brief Top left corner
	constexpr point_type top_left( void ) const { return _position; }

	/// @brief Top right corner
	constexpr point_type top_right( void ) const { return point_type( x2(), y1() ); }

	/// @brief Top center point
	constexpr point_type top_center( void ) const { return point_type( ( x1() + x2() ) / coord_type(2), y1() ); }

	/// @brief Bottom left corner
	constexpr point_type bottom_left( void ) const { return point_type( x1(), y2() ); }

	/// @brief Bottom right corner
	constexpr point_type bottom_right( void ) const { return point_type( x2(), y2() ); }

	/// @brief Move the rectangle
	void set_center( const point_type &p )
	{
		_position = _position + ( p - center() );
	}

	/// @brief Center of rectangle
	constexpr point_type center( void ) const { return point_type( ( x1() + x2() ) / coord_type(2), ( y1() + y2() ) / coord_type(2) ); }

	/// @brief Radius of rectangle
	coord_type radius( void ) const { return std::min( width(), height() ) / coord_type(2); }

	/// @brief Set position and size
	void set( const point_type &p, const size_type &s )
	{
		_position = p;
		_extent = s;
		fix_extent();
	}
	void set( coord_type x, coord_type y, coord_type w, coord_type h )
	{
		_position.set( x, y );
		_extent.set( w, h );
		fix_extent();
	}
	void set( const rect &r )
	{
		_position = r.position();
		_extent = r.extent();
		// no need to re-fix the extent - just a convenience to make it like a copy
	}

	/// @brief Set x position
	void set_x( coord_type x ) { _position[0] = x; }

	/// @brief Set y position
	void set_y( coord_type y ) { _position[1] = y; }

	/// @brief Set width
	void set_width( coord_type w ) { _extent.set_width( w ); fix_extent(); }

	/// @brief Set height
	void set_height( coord_type h ) { _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( coord_type w, coord_type h ) { _extent.set_width( w ); _extent.set_height( h ); fix_extent(); }

	/// @brief Set size
	void set_size( const size_type &s ) { _extent = s; }

	/// @brief Set left side
	void set_x1( coord_type x ) { _position[0] = x; }

	/// @brief Set top side
	void set_y1( coord_type y ) { _position[1] = y; }

	/// @brief Set right side
	void set_x2( coord_type x ) { _extent.set_width( x - x1() ); }

	/// @brief Set bottom side
	void set_y2( coord_type y ) { _extent.set_height( y - y1() ); }

	/// @brief Set left and right sides
	void set_horizontal( coord_type x1, coord_type x2 )
	{
		_position[0] = std::min( x1, x2 );
		_extent.set_width( coord_type( std::abs( x2 - x1 ) ) + kEndOff );
	}

	/// @brief Set top and bottom sides
	void set_vertical( coord_type y1, coord_type y2 )
	{
		_position[1] = std::min( y1, y2 );
		_extent.set_height( coord_type( std::abs( y2 - y1 ) ) + kEndOff );
	}

	/// @brief Set position
	void set_position( const point_type &p ) { _position = p; }

	/// @brief Set size
	void set_extent( const size_type &s ) { _extent = s; fix_extent(); }

	/// @brief Move rectangle by x, y
	void move_by( coord_type x, coord_type y ) { _position.move_by( x, y ); }

	/// @brief Set size
	void set_extent( coord_type w, coord_type h ) { _extent.set( w, h ); }

	/// @brief Trim rectangle on all sides
	void trim( coord_type l, coord_type r, coord_type t, coord_type b ) { _extent.shrink( l + r, t + b ); _position.move_by( l, t ); }

	/// @brief Is x,y contained in the rectangle?
	bool contains( coord_type x, coord_type y ) const
	{
		if ( x < x1() || x > x2() )
			return false;
		if ( y < y1() || y > y2() )
			return false;
		return true;
	}

	/// @brief Is the point contained in the rectangle?
	bool contains( const point_type &p ) const { return contains( p[0], p[1] ); }

	/// @brief Shrink the rectangle
	void shrink( coord_type left, coord_type right, coord_type top, coord_type bottom )
	{
		_position.move_by( left, top );
		_extent.shrink( left + right, top + bottom );
		fix_extent();
	}

	/// @brief Grow the rectangle
	void grow( coord_type left, coord_type right, coord_type top, coord_type bottom )
	{
		_position.move_by( -left, -top );
		_extent.grow( left + right, top + bottom );
		fix_extent();
	}

	void include( const rect &other )
	{
		coord_type xx1 = std::min( x1(), other.x1() );
		coord_type yy1 = std::min( y1(), other.y1() );
		coord_type xx2 = std::max( x2(), other.x2() );
		coord_type yy2 = std::max( y2(), other.y2() );

		set_horizontal( xx1, xx2 );
		set_vertical( yy1, yy2 );
	}

	void clip( const rect &other )
	{
		coord_type xx1 = std::max( x1(), other.x1() );
		coord_type yy1 = std::max( y1(), other.y1() );
		coord_type xx2 = std::min( x2(), other.x2() );
		coord_type yy2 = std::min( y2(), other.y2() );

		if ( xx1 < xx2 )
			set_horizontal( xx1, xx2 );
		else
			set_horizontal( xx1, xx1 );

		if ( yy1 < yy2 )
			set_vertical( yy1, yy2 );
		else
			set_vertical( yy1, yy1 );
	}

	bool empty( void ) const
	{
		return _extent.empty();
	}

	rect round( void ) const
	{
		rect r;
		r.set_x1( std::floor( x1() ) );
		r.set_y1( std::floor( y1() ) );
		r.set_x2( std::ceil( x2() ) );
		r.set_y2( std::ceil( y2() ) );
		return r;
	}
private:
	void fix_extent( void )
	{
		_position.move_by( std::min( _extent.w(), coord_type(0) ), std::min( _extent.h(), coord_type(0) ) );
		_extent.set( coord_type( std::abs( _extent.w() ) ),
					 coord_type( std::abs( _extent.h() ) ) );
	}

	point_type _position;
	size_type _extent;
};

////////////////////////////////////////

/// @brief Output operator for rectangle.
template <typename T>
inline std::ostream &operator<<( std::ostream &out, const rect<T> &r )
{
	out << r.position() << ' ' << r.extent();
	return out;
}

template <typename T>
inline decltype(distance(point<T, 2>(), point<T, 2>()))
distance( const rect<T> &r, const rect<T> &other )
{
	auto d1 = distance( r.top_left(), other.top_left() );
	auto d2 = distance( r.top_right(), other.top_right() );
	auto d3 = distance( r.bottom_left(), other.bottom_left() );
	auto d4 = distance( r.bottom_right(), other.bottom_right() );
	return std::max( std::max( d1, d2 ), std::max( d3, d4 ) );
}


using frect = rect<float>;
using drect = rect<double>;
using irect = rect<int32_t>;
using lrect = rect<int64_t>;

////////////////////////////////////////

} // namespace base
