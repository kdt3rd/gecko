
#pragma once

#include <core/contract.h>
#include <core/direction.h>
#include <core/orientation.h>
#include <memory>
#include <iostream>

namespace layout
{

////////////////////////////////////////

/// @brief Layout constraint.
///
/// Constraints on a layout.
/// Constraints are generally one dimensional (either horizontal or vertical).
/// A layout is made of several constraints working together.
template<typename container>
class constraint
{
public:
	constraint( void )
	{
	}

	virtual ~constraint( void )
	{
	}

	virtual void recompute_minimum( container &a ) = 0;
	virtual void recompute_constraint( container &a ) = 0;
};

////////////////////////////////////////

template<typename list, typename area>
void overlap_minimum( list &areas, area &master, orientation orient, double pad1 = 0.0, double pad2 = 0.0 )
{
	if ( orient == orientation::HORIZONTAL )
	{
		double w = 0.0;
		for ( auto &a: areas )
			w = std::max( w, a->minimum_width() );
		master.set_minimum_width( w + pad1 + pad2 );
	}
	else
	{
		double h = 0.0;
		for ( auto &a: areas )
			h = std::max( h, a->minimum_height() );
		master.set_minimum_height( h + pad1 + pad2 );
	}
}

template<typename list, typename area>
void overlap_constraint( list &areas, area &master, orientation orient, double pad1 = 0.0, double pad2 = 0.0 )
{
	if ( orient == orientation::HORIZONTAL )
	{
		double x = master.x1() + pad1;
		double w = master.width() - pad1 - pad2;
		for ( auto &a: areas )
			a->set_horizontal( x, x + w );
	}
	else
	{
		double y = master.y1() + pad1;
		double h = master.height() - pad1 - pad2;
		for ( auto &a: areas )
			a->set_vertical( y, y + h );
	}
}

template<typename list, typename area>
void flow_minimum( list &areas, area &master, direction dir, double spacing = 0.0, double pad1 = 0.0, double pad2 = 0.0 )
{
	if ( dir == direction::LEFT || dir == direction::RIGHT )
	{
		double w = 0.0;
		for ( auto &a: areas )
			w += a->minimum_width();
		master.set_minimum_width( w + pad1 + pad2 + ( spacing * ( areas.size() - 1 ) ) );
	}
	else
	{
		double h = 0.0;
		for ( auto &a: areas )
			h += a->minimum_height();
		master.set_minimum_height( h + pad1 + pad2 + ( spacing * ( areas.size() - 1 ) ) );
	}
}

template<typename list, typename wlist, typename area>
void xflow_constraint( list &areas, wlist &weights, area &master, direction dir, double spacing = 0.0, double pad1 = 0.0, double pad2 = 0.0 )
{
	double t = 0.0;
	double s = pad1 + pad2 + ( spacing * ( areas.size() - 1 ) ); 
	if ( dir == direction::LEFT || dir == direction::RIGHT )
	{
		for ( size_t i = 0; i < areas.size(); ++i )
		{
			t += weights[i];
			s += areas[i]->minimum_width();
		}

		double x = master.x1() + pad1;
		double extra = std::max( master.width() - s, 0.0 );
		if ( dir == direction::RIGHT )
		{
			for ( size_t i = 0; i < areas.size(); ++i )
			{
				auto &a = areas[i];
				double w = a->minimum_width();
				if ( t > 0.0 )
					w += extra * weights[i] / t;
				a->set_horizontal( x, x + w );
				x += w + spacing;
			}
		}
		else
		{
			for ( size_t i = areas.size(); i > 0; --i )
			{
				auto &a = areas[i-1];
				double w = a->minimum_width();
				if ( t > 0.0 )
					w += extra * weights[i-1] / t;
				a->set_horizontal( x, x + w );
				x += w + spacing;
			}
		}
	}
	else
	{
		for ( size_t i = 0; i < areas.size(); ++i )
		{
			t += weights[i];
			s += areas[i]->minimum_height();
		}

		double y = master.y1() + pad1;
		double extra = std::max( master.height() - s, 0.0 );
		if ( dir == direction::DOWN )
		{
			for ( size_t i = 0; i < areas.size(); ++i )
			{
				auto &a = areas[i];
				double h = a->minimum_height();
				if ( t > 0.0 )
					h += extra * weights[i] / t;
				a->set_vertical( y, y + h );
				y += h + spacing;
			}
		}
		else
		{
			for ( size_t i = areas.size(); i > 0; --i )
			{
				auto &a = areas[i-1];
				double h = a->minimum_height();
				if ( t > 0.0 )
					h += extra * weights[i-1] / t;
				a->set_vertical( y, y + h );
				y += h + spacing;
			}
		}
	}
}

////////////////////////////////////////

}
