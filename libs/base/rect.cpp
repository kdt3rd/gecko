//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <algorithm>
#include "rect.h"

namespace base
{

////////////////////////////////////////

void rect::set_horizontal( coord_type x1, coord_type x2 )
{
	_position.set_x( std::min( x1, x2 ) );
	_extent.set_width( std::abs( x2 - x1 ) + coord_type(1) );
}

////////////////////////////////////////

void rect::set_vertical( coord_type y1, coord_type y2 )
{
	_position.set_y( std::min( y1, y2 ) );
	_extent.set_height( std::abs( y2 - y1 ) + coord_type(1) );
}

////////////////////////////////////////

bool rect::contains( coord_type x, coord_type y ) const
{
	if ( x < x1() || x > x2() )
		return false;

	if ( y < y1() || y > y2() )
		return false;

	return true;
}

////////////////////////////////////////

void rect::shrink( coord_type left, coord_type right, coord_type top, coord_type bottom )
{
	_position.move_by( left, top );
	_extent.shrink( left + right, top + bottom );
	fix_extent();
}

////////////////////////////////////////

void rect::grow( coord_type left, coord_type right, coord_type top, coord_type bottom )
{
	_position.move_by( -left, -top );
	_extent.grow( left + right, top + bottom );
	fix_extent();
}

////////////////////////////////////////

void rect::include( rect &other )
{
	coord_type xx1 = std::min( x1(), other.x1() );
	coord_type yy1 = std::min( y1(), other.y1() );
	coord_type xx2 = std::max( x2(), other.x2() );
	coord_type yy2 = std::max( y2(), other.y2() );

	set_horizontal( xx1, xx2 );
	set_vertical( yy1, yy2 );
}

////////////////////////////////////////

double rect::distance( rect &other ) const
{
	double d1 = base::point::distance( top_left(), other.top_left() );
	double d2 = base::point::distance( top_right(), other.top_right() );
	double d3 = base::point::distance( bottom_left(), other.bottom_left() );
	double d4 = base::point::distance( bottom_right(), other.bottom_right() );
	return std::max( std::max( d1, d2 ), std::max( d3, d4 ) );
}

////////////////////////////////////////

}

