
#include "form_layout.h"

namespace layout
{

////////////////////////////////////////

form_layout::form_layout( const std::shared_ptr<draw::area> &a, direction dir )
	: _container( a ),
		_left_area( std::make_shared<draw::area>() ), _right_area( std::make_shared<draw::area>() ),
		_left( _left_area, orientation::HORIZONTAL ), _right( _right_area, orientation::HORIZONTAL ),
		_down( a, direction::DOWN ),
		_columns( a, dir )
{
	_columns.add_area( _left_area, 0.0 );
	_columns.add_area( _right_area, 1.0 );
}

////////////////////////////////////////

std::pair<std::shared_ptr<draw::area>,std::shared_ptr<draw::area>> form_layout::new_row( void )
{
	auto box = std::make_shared<draw::area>();
	auto a = std::make_shared<draw::area>();
	auto b = std::make_shared<draw::area>();

	_rows.emplace_back( box, orientation::VERTICAL );
	_rows.back().add_area( a );
	_rows.back().add_area( b );

	_down.add_area( box );

	_left.add_area( a );
	_right.add_area( b );

	return std::make_pair( a, b );
}

////////////////////////////////////////

void form_layout::recompute_minimum( void )
{
	for ( auto t: _rows )
		t.recompute_minimum();
	_left.recompute_minimum();
	_right.recompute_minimum();
	_down.recompute_minimum();
	_columns.recompute_minimum();
}

////////////////////////////////////////

void form_layout::recompute_layout( void )
{
	_columns.recompute_constraint();
	_down.recompute_constraint();
	_right.recompute_constraint();
	_left.recompute_constraint();
	for ( auto t: _rows )
		t.recompute_constraint();
}

////////////////////////////////////////

}

