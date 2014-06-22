
#include <algorithm>
#include "rect.h"

namespace core
{

////////////////////////////////////////

void rect::set_horizontal( double x1, double x2 )
{
	_position.set_x( std::min( x1, x2 ) );
	_size.set_width( std::abs( x2 - x1 ) + 1.0 );
}

////////////////////////////////////////

void rect::set_vertical( double y1, double y2 )
{
	_position.set_y( std::min( y1, y2 ) );
	_size.set_height( std::abs( y2 - y1 ) + 1.0 );
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
	_size.shrink( left + right, top + bottom );
	fix_size();
}

////////////////////////////////////////

void rect::grow( double left, double right, double top, double bottom )
{
	_position.move_by( -left, -top );
	_size.grow( left + right, top + bottom );
	fix_size();
}

////////////////////////////////////////

}

