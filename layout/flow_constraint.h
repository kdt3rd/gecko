
#pragma once

#include <vector>
#include <core/reverse.h>
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Flow areas in a direction.
template<typename container, typename area>
class flow_constraint : public constraint<container>
{
public:
	flow_constraint( direction d = direction::RIGHT )
		: _dir( d )
	{
	}

	void add_area( const std::shared_ptr<area> &a, double weight = 0.0 )
	{
		_areas.emplace_back( a, weight );
	}

	void set_direction( direction d )
	{
		_dir = d;
	}

	direction get_direction( void ) const
	{
		return _dir;
	}

	void set_spacing( double s )
	{
		_spacing = s;
	}

	void set_pad( double start, double end )
	{
		_pad = { start, end };
	}

	void recompute_minimum( container &master )
	{
		if ( _dir == direction::LEFT || _dir == direction::RIGHT )
		{
			double w = 0.0;
			for ( auto a: _areas )
				w += a.first->minimum_width();
			master.set_minimum_width( w + _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) ) );
		}
		else
		{
			double h = 0.0;
			for ( auto a: _areas )
				h += a.first->minimum_height();
			master.set_minimum_height( h + _pad.first + _pad.second + ( _spacing * ( _areas.size() - 1 ) ) );
		}
	}

	void recompute_constraint( container &master )
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

			double x = master.x1() + _pad.first;
			double extra = std::max( master.width() - w, 0.0 );
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

			double y = master.y1() + _pad.first;
			double extra = std::max( master.height() - h, 0.0 );
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

private:
	direction _dir = direction::RIGHT;
	double _spacing = 0.0;
	std::pair<double,double> _pad = { 0.0, 0.0 };
	std::vector<std::pair<std::shared_ptr<area>,double>> _areas;
};

////////////////////////////////////////

}

