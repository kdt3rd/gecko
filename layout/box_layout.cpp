
#include "box_layout.h"

namespace layout
{

////////////////////////////////////////

box_layout::box_layout( const std::shared_ptr<area> &c, direction dir )
	: _container( c ), _flow( c ), _cross( c )
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

std::shared_ptr<area> box_layout::new_area( double w )
{
	auto a = std::make_shared<area>();
	add_area( a, w );
	return a;
}

////////////////////////////////////////

void box_layout::add_area( const std::shared_ptr<area> &a, double w )
{
	_areas.push_back( a );
	_flow.add_area( a, w );
	_cross.add_area( a );
}

////////////////////////////////////////

void box_layout::recompute_minimum( void )
{
	_flow.recompute_minimum();
	_cross.recompute_minimum();
}

////////////////////////////////////////

void box_layout::recompute_layout( void )
{
	_flow.recompute_constraint();
	_cross.recompute_constraint();
}

////////////////////////////////////////

}
