
#include "form_layout.h"

namespace layout
{

////////////////////////////////////////

form_layout::form_layout( direction dir )
	: _left_area( std::make_shared<area>() ), _right_area( std::make_shared<area>() ),
		_left( orientation::HORIZONTAL ), _right( orientation::HORIZONTAL ),
		_down( direction::DOWN ), _columns( dir )
{
	_columns.add_area( _left_area, 0.0 );
	_columns.add_area( _right_area, 1.0 );
}

////////////////////////////////////////

void form_layout::set_pad( double left, double right, double top, double bottom )
{
	_left.set_pad( left, 0.0 );
	_right.set_pad( 0.0, right );
	_down.set_pad( top, bottom );
}

////////////////////////////////////////

void form_layout::set_spacing( double horiz, double vert )
{
	_columns.set_spacing( horiz );
	_down.set_spacing( vert );
}


////////////////////////////////////////

/*
std::pair<std::shared_ptr<area>,std::shared_ptr<area>> form_layout::new_row( void )
{
	auto a = std::make_shared<area>();
	auto b = std::make_shared<area>();

	add_row( a, b );

	return std::make_pair( a, b );
}
*/

////////////////////////////////////////

void form_layout::add( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b )
{
	auto box = std::make_shared<area>();

	_rows.emplace_back( tight_constraint( orientation::VERTICAL ), area() );
	_rows.back().first.add_area( a );
	_rows.back().first.add_area( b );

	_down.add_area( box );

	_left.add_area( a );
	_right.add_area( b );
}

////////////////////////////////////////

void form_layout::recompute_minimum( area &master )
{
	for ( auto t: _rows )
		t.first.recompute_minimum( t.second );
	_left.recompute_minimum( *_left_area );
	_right.recompute_minimum( *_right_area );
	_down.recompute_minimum( master );
	_columns.recompute_minimum( master );
}

////////////////////////////////////////

void form_layout::recompute_layout( area &master )
{
	_columns.recompute_constraint( master );
	_down.recompute_constraint( master );
	_right.recompute_constraint( *_right_area );
	_left.recompute_constraint( *_left_area );
	for ( auto t: _rows )
		t.first.recompute_constraint( t.second );
}

////////////////////////////////////////

}

