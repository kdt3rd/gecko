
#include "form_layout.h"

////////////////////////////////////////

form_layout::form_layout( const std::shared_ptr<container> &c, const std::shared_ptr<area> &a, direction dir )
	: _container( c ),
		_left_area( c->new_area() ), _right_area( c->new_area() ),
		_left( _left_area, orientation::HORIZONTAL ), _right( _right_area, orientation::HORIZONTAL ),
		_down( a, direction::DOWN ),
		_columns( a, dir )
{
	_columns.add_area( _left_area, 0.0 );
	_columns.add_area( _right_area, 1.0 );
}

////////////////////////////////////////

std::pair<std::shared_ptr<area>,std::shared_ptr<area>> form_layout::new_line( void )
{
	auto box = _container->new_area();
	auto a = _container->new_area();
	auto b = _container->new_area();

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
