
#include "box_layout.h"

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

std::shared_ptr<area> box_layout::new_area( double w )
{
	auto a = std::make_shared<area>();
	_areas.push_back( a );
	_flow.add_area( a, w );
	_cross.add_area( a );

	return a;
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

