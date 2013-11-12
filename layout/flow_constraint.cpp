
#include <core/reverse.h>
#include "flow_constraint.h"

namespace layout
{

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
		_area->set_minimum_width( w + _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) ) );
	}
	else
	{
		double h = 0.0;
		for ( auto a: _areas )
			h += a.first->minimum_height();
		_area->set_minimum_height( h + _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) ) );
	}
}

////////////////////////////////////////

void flow_constraint::recompute_constraint( void )
{
	if ( _dir == direction::LEFT || _dir == direction::RIGHT )
	{
		double t = 0.0;
		double w = _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) ); 
		for ( auto a: _areas )
		{
			t += a.second;
			w += a.first->minimum_width();
		}

		double x = _area->x1() + _pad.first;
		double extra = std::max( _area->width() - w, 0.0 );
		if ( _dir == direction::RIGHT )
		{
			for ( auto a: _areas )
			{
				double w = a.first->minimum_width();
				if ( t > 0.0 )
					w += extra * a.second / t;
				a.first->set_horizontal( x, x + w );
				x += w + _spacing;
			}
		}
		else
		{
			for ( auto a: reverse( _areas ) )
			{
				double w = a.first->minimum_width();
				if ( t > 0.0 )
					w += extra * a.second / t;
				a.first->set_horizontal( x, x + w );
				x += w + _spacing;
			}
		}
	}
	else
	{
		double t = 0.0;
		double h = _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) );
		for ( auto a: _areas )
		{
			t += a.second;
			h += a.first->minimum_height();
		}

		double y = _area->y1() + _pad.first;
		double extra = std::max( _area->height() - h, 0.0 );
		if ( _dir == direction::DOWN )
		{
			for ( auto a: _areas )
			{
				double h = a.first->minimum_height();
				if ( t > 0.0 )
					h += extra * a.second / t;
				a.first->set_vertical( y, y + h );
				y += h + _spacing;
			}
		}
		else
		{
			for ( auto a: reverse( _areas ) )
			{
				double h = a.first->minimum_height();
				if ( t > 0.0 )
					h += extra * a.second / t;
				a.first->set_vertical( y, y + h );
				y += h + _spacing;
			}
		}
	}
}

////////////////////////////////////////

}

