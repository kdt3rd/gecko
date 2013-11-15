
#include "box_layout.h"

namespace layout
{

////////////////////////////////////////

box_layout::box_layout( direction dir )
{
	set_direction( dir );
}

////////////////////////////////////////

void box_layout::set_direction( direction d )
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

////////////////////////////////////////

void box_layout::set_pad( double left, double right, double top, double bottom )
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

////////////////////////////////////////

void box_layout::set_spacing( double horiz, double vert )
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

////////////////////////////////////////

void box_layout::add( const std::shared_ptr<area> &a, double w )
{
	_flow.add_area( a, w );
	_cross.add_area( a );
}

////////////////////////////////////////

void box_layout::recompute_minimum( area &master )
{
	_flow.recompute_minimum( master );
	_cross.recompute_minimum( master );
}

////////////////////////////////////////

void box_layout::recompute_layout( area &master )
{
	_flow.recompute_constraint( master );
	_cross.recompute_constraint( master );
}

////////////////////////////////////////

}
