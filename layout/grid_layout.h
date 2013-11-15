
#pragma once

#include <memory>
#include "layout.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Grid layout.
///
/// Lays out areas in a grid of rows and columns.
template<typename container, typename area>
class grid_layout : public layout<container, area>
{
	typedef tight_constraint<simple_area,area> tconstraint;

public:
	grid_layout( void );

	void add_row( const std::vector<std::shared_ptr<area>> &add, double w )
	{
		precondition( add.size() == _columns.cells.size(), "invalid row size" );
		_rows.cells.emplace_back( std::make_pair( std::make_shared<tconstraint>( orientation::VERTICAL ), std::make_shared<simple_area>() ) );
		_rows.box.add_area( _rows.cells.back().second, w );
		auto &row = _rows.cells.back();
		for ( size_t i = 0; i < add.size(); ++i )
		{
			row.first->add_area( add[i] );
			_columns.cells[i].first->add_area( add[i] );
		}
	}

	void add_column( const std::vector<std::shared_ptr<area>> &add, double w )
	{
		precondition( add.size() == _rows.cells.size(), "invalid columns size" );
		_columns.cells.emplace_back( std::make_pair( std::make_shared<tconstraint>( orientation::HORIZONTAL ), std::make_shared<simple_area>() ) );
		_columns.box.add_area( _columns.cells.back().second, w );
		auto &col = _columns.cells.back().first;
		for ( size_t i = 0; i < add.size(); ++i )
		{
			col->add_area( add[i] );
			_rows.cells[i].first->add_area( add[i] );
		}
	}

	virtual void set_pad( double left, double right, double top, double bottom )
	{
		_rows.box.set_pad( left, right );
		_columns.box.set_pad( top, bottom );
	}

	virtual void set_spacing( double horiz, double vert )
	{
		_rows.box.set_spacing( horiz );
		_columns.box.set_spacing( vert );
	}

	virtual void recompute_minimum( container &master )
	{
		for ( auto r: _rows.cells )
			r.first->recompute_minimum( *r.second );
		for ( auto c: _columns.cells )
			c.first->recompute_minimum( *c.second );

		_rows.box.recompute_minimum( master );
		_columns.box.recompute_minimum( master );
	}


	virtual void recompute_layout( container &master )
	{
		_rows.box.recompute_constraint( master );
		_columns.box.recompute_constraint( master );

		for ( auto r: _rows.cells )
			r.first->recompute_constraint( *r.second );
		for ( auto c: _columns.cells )
			c.first->recompute_constraint( *c.second );
	}

private:
	std::shared_ptr<area> _container;

	struct group
	{
		group( void )
			: a( std::make_shared<simple_area>() )
		{
		}

		std::shared_ptr<simple_area> a;
		flow_constraint<container,simple_area> box;
		std::vector<std::pair<std::shared_ptr<tconstraint>,std::shared_ptr<simple_area>>> cells;
	};

	group _rows;
	group _columns;
};

////////////////////////////////////////

}

