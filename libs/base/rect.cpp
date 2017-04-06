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

void rect::set_horizontal( double x1, double x2 )
{
	_position.set_x( std::min( x1, x2 ) );
	_extent.set_width( std::abs( x2 - x1 ) + 1.0 );
}

////////////////////////////////////////

void rect::set_vertical( double y1, double y2 )
{
	_position.set_y( std::min( y1, y2 ) );
	_extent.set_height( std::abs( y2 - y1 ) + 1.0 );
}

////////////////////////////////////////

bool rect::contains( double x, double y ) const
{
	if ( x < x1() || x > x2() )
		return false;

	if ( y < y1() || y > y2() )
		return false;

	return true;
}

////////////////////////////////////////

void rect::shrink( double left, double right, double top, double bottom )
{
	_position.move_by( left, top );
	_extent.shrink( left + right, top + bottom );
	fix_extent();
}

////////////////////////////////////////

void rect::grow( double left, double right, double top, double bottom )
{
	_position.move_by( -left, -top );
	_extent.grow( left + right, top + bottom );
	fix_extent();
}

////////////////////////////////////////

}

