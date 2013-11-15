
#include "grid_layout.h"

namespace layout
{

////////////////////////////////////////

grid_layout::grid_layout( void )
{
	_rows.box.set_direction( direction::DOWN );
	_columns.box.set_direction( direction::RIGHT );
}

////////////////////////////////////////

void grid_layout::add_row( const std::vector<std::shared_ptr<area>> &add, double w )
{
	precondition( add.size() == _columns.cells.size(), "invalid row size" );
	_rows.cells.emplace_back( std::make_pair( std::make_shared<tight_constraint>( orientation::VERTICAL ), std::make_shared<area>() ) );
	_rows.box.add_area( _rows.cells.back().second, w );
	auto &row = _rows.cells.back();
	for ( size_t i = 0; i < add.size(); ++i )
	{
		row.first->add_area( add[i] );
		_columns.cells[i].first->add_area( add[i] );
	}
}

////////////////////////////////////////

void grid_layout::add_column( const std::vector<std::shared_ptr<area>> &add, double w )
{
	precondition( add.size() == _rows.cells.size(), "invalid columns size" );
	_columns.cells.emplace_back( std::make_pair( std::make_shared<tight_constraint>( orientation::HORIZONTAL ), std::make_shared<area>() ) );
	_columns.box.add_area( _columns.cells.back().second, w );
	auto &col = _columns.cells.back().first;
	for ( size_t i = 0; i < add.size(); ++i )
	{
		col->add_area( add[i] );
		_rows.cells[i].first->add_area( add[i] );
	}
}

////////////////////////////////////////

void grid_layout::set_pad( double left, double right, double top, double bottom )
{
	_rows.box.set_pad( left, right );
	_columns.box.set_pad( top, bottom );
}

////////////////////////////////////////

void grid_layout::set_spacing( double horiz, double vert )
{
	_rows.box.set_spacing( horiz );
	_columns.box.set_spacing( vert );
}

////////////////////////////////////////

void grid_layout::recompute_minimum( area &master )
{
	for ( auto r: _rows.cells )
		r.first->recompute_minimum( *r.second );
	for ( auto c: _columns.cells )
		c.first->recompute_minimum( *c.second );

	_rows.box.recompute_minimum( master );
	_columns.box.recompute_minimum( master );
}

////////////////////////////////////////

void grid_layout::recompute_layout( area &master )
{
	_rows.box.recompute_constraint( master );
	_columns.box.recompute_constraint( master );

	for ( auto r: _rows.cells )
		r.first->recompute_constraint( *r.second );
	for ( auto c: _columns.cells )
		c.first->recompute_constraint( *c.second );
}

////////////////////////////////////////

}

