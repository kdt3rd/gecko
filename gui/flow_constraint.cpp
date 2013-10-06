
#include "flow_constraint.h"

////////////////////////////////////////

flow_constraint::flow_constraint( const std::shared_ptr<area> &a, direction d )
	: constraint( a ), _dir( d )
{
}

////////////////////////////////////////

void flow_constraint::add_area( const std::shared_ptr<area> &a, double weight )
{
	_areas.emplace_back( a, weight );
}

////////////////////////////////////////

void flow_constraint::recompute_minimum( void )
{
	if ( _dir == direction::LEFT || _dir == direction::RIGHT )
	{
		double w = 0.0;
		for ( auto a: _areas )
			w += a.first->minimum_width();
		_area->set_minimum_width( w );
	}
	else
	{
		double h = 0.0;
		for ( auto a: _areas )
			h += a.first->minimum_height();
		_area->set_minimum_height( h );
	}
}

////////////////////////////////////////

void flow_constraint::recompute_constraint( void )
{
	if ( _dir == direction::LEFT || _dir == direction::RIGHT )
	{
		double t = 0.0;
		double w = 0.0;
		for ( auto a: _areas )
		{
			t += a.second;
			w += a.first->minimum_width();
		}

		double x = _area->x1();
		double extra = _area->width() - w;
		for ( auto a: _areas )
		{
			double w = a.first->minimum_width();
			if ( t > 0.0 )
				w += extra * a.second / t;
			a.first->set_horizontal( x, x + w );
			x += w;
		}
	}
	else
	{
		double t = 0.0;
		double h = 0.0;
		for ( auto a: _areas )
		{
			t += a.second;
			h += a.first->minimum_height();
		}

		double y = _area->y1();
		double extra = _area->height() - h;
		for ( auto a: _areas )
		{
			double h = a.first->minimum_height();
			if ( t > 0.0 )
				h += extra * a.second / t;
			a.first->set_vertical( y, y + h );
			y += h;
		}
	}
}

////////////////////////////////////////
