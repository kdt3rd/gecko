
#include "tight_constraint.h"

////////////////////////////////////////

tight_constraint::tight_constraint( const std::shared_ptr<area> &a, orientation o )
	: constraint( a ), _orient( o )
{
}

////////////////////////////////////////

void tight_constraint::add_area( const std::shared_ptr<area> &a )
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
		_area->set_minimum_width( w );
	}
	else
	{
		double h = 0.0;
		for ( auto a: _areas )
			h = std::max( h, a->minimum_height() );
		_area->set_minimum_height( h );
	}
}

////////////////////////////////////////

void tight_constraint::recompute_constraint( void )
{
	if ( _orient == orientation::HORIZONTAL )
	{
		double x = _area->x1();
		double w = _area->width();
		for ( auto a: _areas )
			a->set_horizontal( x, x + w );
	}
	else
	{
		double y = _area->y1();
		double h = _area->height();
		for ( auto a: _areas )
			a->set_vertical( y, y + h );
	}
}

////////////////////////////////////////
