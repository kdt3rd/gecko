
#pragma once

#include <memory>
#include "layout.h"
#include "simple_area.h"
#include "tight_constraint.h"
#include "flow_constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Form layout.
///
/// Lays out areas in 2 columns, one for labels, another for widgets.
template<typename container, typename area>
class form_layout : public layout<container,area>
{
public:
	form_layout( direction dir = direction::RIGHT )
		: _left_area( std::make_shared<simple_area>() ), _right_area( std::make_shared<simple_area>() ),
			_left( orientation::HORIZONTAL ), _right( orientation::HORIZONTAL ),
			_down( direction::DOWN ), _columns( dir )
	{
		_columns.add_area( _left_area, 0.0 );
		_columns.add_area( _right_area, 1.0 );
	}

	void set_direction( direction d )
	{
		_columns.set_direction( d );
	}

	virtual void set_pad( double left, double right, double top, double bottom )
	{
		_left.set_pad( left, 0.0 );
		_right.set_pad( 0.0, right );
		_down.set_pad( top, bottom );
	}

	virtual void set_spacing( double horiz, double vert )
	{
		_columns.set_spacing( horiz );
		_down.set_spacing( vert );
	}

	void add( const std::shared_ptr<area> &l, const std::shared_ptr<area> &a )
	{
		auto box = std::make_shared<simple_area>();

		_rows.emplace_back( tight_constraint<simple_area,area>( orientation::VERTICAL ), box );
		_rows.back().first.add_area( l );
		_rows.back().first.add_area( a );

		_down.add_area( box );

		_left.add_area( l );
		_right.add_area( a );
	}

	virtual void recompute_minimum( container &master )
	{
		for ( auto t: _rows )
			t.first.recompute_minimum( *t.second );
		_left.recompute_minimum( *_left_area );
		_right.recompute_minimum( *_right_area );
		_down.recompute_minimum( master );
		_columns.recompute_minimum( master );
	}

	virtual void recompute_layout( container &master )
	{
		_columns.recompute_constraint( master );
		_down.recompute_constraint( master );
		_right.recompute_constraint( *_right_area );
		_left.recompute_constraint( *_left_area );
		for ( auto t: _rows )
			t.first.recompute_constraint( *t.second );
	}

private:
	std::shared_ptr<simple_area> _left_area;
	std::shared_ptr<simple_area> _right_area;

	tight_constraint<simple_area,area> _left, _right;
	flow_constraint<container,simple_area> _down, _columns;
	std::vector<std::pair<tight_constraint<simple_area,area>,std::shared_ptr<simple_area>>> _rows;
};

////////////////////////////////////////

}

