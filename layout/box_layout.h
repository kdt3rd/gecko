
#pragma once

#include <memory>
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Box layout.
///
/// Organizes areas in single row (or column).
/// Each area can have a weight.  Extra space will be distributed among the areas by weight.
template<typename container, typename area>
class box_layout : public layout<container,area>
{
public:
	box_layout( direction dir = direction::RIGHT )
	{
		set_direction( dir );
	}

	void set_direction( direction d )
	{
		_flow.set_direction( d );
		switch ( d )
		{
			case direction::LEFT:
			case direction::RIGHT:
				_cross.set_orientation( orientation::VERTICAL );
				break;

			case direction::UP:
			case direction::DOWN:
				_cross.set_orientation( orientation::HORIZONTAL );
				break;
		}
	}

	virtual void set_pad( double left, double right, double top, double bottom )
	{
		switch ( _flow.get_direction() )
		{
			case direction::LEFT:
			case direction::RIGHT:
				_flow.set_pad( left, right );
				_cross.set_pad( top, bottom );
				break;

			case direction::UP:
			case direction::DOWN:
				_flow.set_pad( top, bottom );
				_cross.set_pad( left, right );
				break;
		}
	}

	virtual void set_spacing( double horiz, double vert )
	{
		switch ( _flow.get_direction() )
		{
			case direction::LEFT:
			case direction::RIGHT:
				_flow.set_spacing( horiz );
				break;

			case direction::UP:
			case direction::DOWN:
				_flow.set_spacing( vert );
				break;
		}
	}

	void add( const std::shared_ptr<area> &a, double weight = 0.0 )
	{
		_flow.add_area( a, weight );
		_cross.add_area( a );
		this->added( a );
	}

	virtual void recompute_minimum( container &master )
	{
		_flow.recompute_minimum( master );
		_cross.recompute_minimum( master );
	}

	virtual void recompute_layout( container &master )
	{
		_flow.recompute_constraint( master );
		_cross.recompute_constraint( master );
	}

private:
	flow_constraint<container, area> _flow;
	tight_constraint<container, area> _cross;
};

////////////////////////////////////////

}

