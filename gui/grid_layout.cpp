
#include "grid_layout.h"

////////////////////////////////////////

grid_layout::grid_layout( const std::shared_ptr<container> &c, const std::shared_ptr<area> &a )
	: _container( c ), _rows( a ), _columns( a )
{
	_rows.box.set_direction( direction::DOWN );
	_columns.box.set_direction( direction::RIGHT );
}

////////////////////////////////////////

void grid_layout::new_row( double w )
{
	auto r = _container->new_area();
	_rows.box.add_area( r, w );
	_rows.cells.emplace_back( r, orientation::VERTICAL );
	auto &row = _rows.cells.back();
	for ( auto &col: _columns.cells )
	{
		auto a = _container->new_area();
		row.add_area( a );
		col.add_area( a );
	}
}

////////////////////////////////////////

void grid_layout::new_column( double w )
{
	auto c = _container->new_area();
	_columns.box.add_area( c, w );
	_columns.cells.emplace_back( c, orientation::HORIZONTAL );
	auto &col = _columns.cells.back();
	for ( auto &row: _rows.cells )
	{
		auto a = _container->new_area();
		col.add_area( a );
		row.add_area( a );
	}
}

////////////////////////////////////////

void grid_layout::recompute_minimum( void )
{
	for ( auto r: _rows.cells )
		r.recompute_minimum();
	for ( auto c: _columns.cells )
		c.recompute_minimum();

	_rows.box.recompute_minimum();
	_columns.box.recompute_minimum();
}

////////////////////////////////////////

void grid_layout::recompute_layout( void )
{
	_rows.box.recompute_constraint();
	_columns.box.recompute_constraint();

	for ( auto r: _rows.cells )
		r.recompute_constraint();
	for ( auto c: _columns.cells )
		c.recompute_constraint();
}

////////////////////////////////////////

