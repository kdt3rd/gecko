
#pragma once

#include <vector>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Tight constraint.
///
/// Superimpose areas on top of each other.
template<typename container, typename area>
class tight_constraint : public constraint<container>
{
public:
	tight_constraint( orientation o = orientation::VERTICAL )
		: _orient( o )
	{
	}

	virtual ~tight_constraint( void )
	{
	}

	void set_orientation( orientation o )
	{
		_orient = o;
	}

	void set_pad( double start, double end )
	{
		_pad = { start, end };
	}

	void add_area( const std::shared_ptr<area> &a )
	{
		_areas.push_back( a );
	}

	std::shared_ptr<area> get_area( size_t i )
	{
		return _areas.at( i );
	}

	void recompute_minimum( container &master )
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

	void recompute_constraint( container &master )
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

private:
	orientation _orient = orientation::VERTICAL;
	std::pair<double,double> _pad = { 0.0, 0.0 };
	std::vector<std::shared_ptr<area>> _areas;
};

////////////////////////////////////////

}

