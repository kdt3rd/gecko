
#include "tight_constraint.h"

namespace layout
{

////////////////////////////////////////

tight_constraint::tight_constraint( const std::shared_ptr<draw::area> &a, orientation o )
	: constraint( a ), _orient( o )
{
}

////////////////////////////////////////

void tight_constraint::add_area( const std::shared_ptr<draw::area> &a )
{
	_areas.push_back( a );
}

////////////////////////////////////////

void tight_constraint::recompute_minimum( void )
{
	if ( _orient == orientation::HORIZONTAL )
	{
		double w = 0.0;
		for ( auto a: _areas )
			w = std::max( w, a->minimum_width() );
		_area->set_minimum_width( w + _pad.first + _pad.second );
	}
	else
	{
		double h = 0.0;
		for ( auto a: _areas )
			h = std::max( h, a->minimum_height() );
		_area->set_minimum_height( h + _pad.first + _pad.second );
	}
}

////////////////////////////////////////

void tight_constraint::recompute_constraint( void )
{
	if ( _orient == orientation::HORIZONTAL )
	{
		double x = _area->x1() + _pad.first;
		double w = _area->width() - _pad.first - _pad.second;
		for ( auto a: _areas )
			a->set_horizontal( x, x + w );
	}
	else
	{
		double y = _area->y1() + _pad.first;
		double h = _area->height() - _pad.first - _pad.second;
		for ( auto a: _areas )
			a->set_vertical( y, y + h );
	}
}

////////////////////////////////////////

}

