
#include "grid_layout.h"

namespace layout
{

////////////////////////////////////////

grid_layout::grid_layout( const std::shared_ptr<area> &c )
	: _container( c ), _rows( c ), _columns( c )
{
	_rows.box.set_direction( direction::DOWN );
	_columns.box.set_direction( direction::RIGHT );
}

////////////////////////////////////////

std::vector<std::shared_ptr<area>> grid_layout::new_row( double w )
{
	std::vector<std::shared_ptr<area>> ret;
	auto r = std::make_shared<area>();
	_rows.box.add_area( r, w );
	_rows.cells.emplace_back( r, orientation::VERTICAL );
	auto &row = _rows.cells.back();
	for ( auto &col: _columns.cells )
	{
		auto a = std::make_shared<area>();
		ret.push_back( a );
		row.add_area( a );
		col.add_area( a );
	}

	return ret;
}

////////////////////////////////////////

void grid_layout::add_row( const std::vector<std::shared_ptr<area>> &add, double w )
{
	precondition( add.size() == _columns.cells.size(), "invalid row size" );
	auto r = std::make_shared<area>();
	_rows.box.add_area( r, w );
	_rows.cells.emplace_back( r, orientation::VERTICAL );
	auto &row = _rows.cells.back();
	for ( size_t i = 0; i < add.size(); ++i )
	{
		row.add_area( add[i] );
		_columns.cells[i].add_area( add[i] );
	}
}

////////////////////////////////////////

std::vector<std::shared_ptr<area>> grid_layout::new_column( double w )
{
	std::vector<std::shared_ptr<area>> ret;
	auto c = std::make_shared<area>();
	_columns.box.add_area( c, w );
	_columns.cells.emplace_back( c, orientation::HORIZONTAL );
	auto &col = _columns.cells.back();
	for ( auto &row: _rows.cells )
	{
		auto a = std::make_shared<area>();
		ret.push_back( a );
		col.add_area( a );
		row.add_area( a );
	}

	return ret;
} 
////////////////////////////////////////

void grid_layout::add_column( const std::vector<std::shared_ptr<area>> &add, double w )
{
	precondition( add.size() == _rows.cells.size(), "invalid columns size" );
	auto c = std::make_shared<area>();
	_columns.box.add_area( c, w );
	_columns.cells.emplace_back( c, orientation::HORIZONTAL );
	auto &col = _columns.cells.back();
	for ( size_t i = 0; i < add.size(); ++i )
	{
		col.add_area( add[i] );
		_rows.cells[i].add_area( add[i] );
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

}

