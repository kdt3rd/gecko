
#include "tight_constraint.h"

namespace layout
{

////////////////////////////////////////

tight_constraint::tight_constraint( orientation o )
	: _orient( o )
{
}

////////////////////////////////////////

void tight_constraint::add_area( const std::shared_ptr<area> &a )
{
	_areas.push_back( a );
}

////////////////////////////////////////

void tight_constraint::recompute_minimum( area &master )
{
	if ( _orient == orientation::HORIZONTAL )
	{
		double w = 0.0;
		for ( auto a: _areas )
			w = std::max( w, a->minimum_width() );
		master.set_minimum_width( w + _pad.first + _pad.second );
	}
	else
	{
		double h = 0.0;
		for ( auto a: _areas )
			h = std::max( h, a->minimum_height() );
		master.set_minimum_height( h + _pad.first + _pad.second );
	}
}

////////////////////////////////////////

void tight_constraint::recompute_constraint( area &master )
{
	if ( _orient == orientation::HORIZONTAL )
	{
		double x = master.x1() + _pad.first;
		double w = master.width() - _pad.first - _pad.second;
		for ( auto a: _areas )
			a->set_horizontal( x, x + w );
	}
	else
	{
		double y = master.y1() + _pad.first;
		double h = master.height() - _pad.first - _pad.second;
		for ( auto a: _areas )
			a->set_vertical( y, y + h );
	}
}

////////////////////////////////////////

}

