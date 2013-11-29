
#pragma once

#include <memory>
#include <array>
#include <core/iterator.h>
#include "layout.h"
#include "simple_area.h"
#include "constraint.h"

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
	form_layout( direction dir = direction::DOWN )
		: _dir( dir )
	{
		_cols.push_back( std::make_shared<simple_area>() );
		_cols.push_back( std::make_shared<simple_area>() );
	}

	void set_direction( direction d )
	{
		_dir = d;
	}

	virtual void set_pad( double left, double right, double top, double bottom )
	{
		_pad[0] = left;
		_pad[1] = right;
		_pad[2] = top;
		_pad[3] = bottom;
	}

	virtual void set_spacing( double horiz, double vert )
	{
		_hspacing = horiz;
		_vspacing = vert;
	}

	void add( const std::shared_ptr<area> &l, const std::shared_ptr<area> &a, double weight = 0.0 )
	{
		_areas.push_back( {{ l, a }}  );
		_weights.push_back( weight );
		_rows.push_back( std::make_shared<simple_area>() );
		this->added( l );
		this->added( a );
	}

	virtual void recompute_minimum( container &master )
	{
		for ( auto &r: _areas )
		{
			r[0]->layout();
			r[1]->layout();
		}

		switch ( _dir )
		{
			case direction::LEFT:
				/*
				flow_minimum( _areas, master, _dir, _hspacing, _pad[0], _pad[1] );
				overlap_minimum( _areas, master, orientation::VERTICAL, _pad[2], _pad[3] );
				*/
				break;

			case direction::RIGHT:
				/*
				flow_minimum( _areas, master, _dir, _hspacing, _pad[1], _pad[0] );
				overlap_minimum( _areas, master, orientation::VERTICAL, _pad[2], _pad[3] );
				*/
				break;

			case direction::UP:
				/*
				flow_minimum( _areas, master, _dir, _vspacing, _pad[2], _pad[3] );
				overlap_minimum( _areas, master, orientation::HORIZONTAL, _pad[0], _pad[1] );
				*/
				break;

			case direction::DOWN:
				for ( size_t c = 0; c < _cols.size(); ++c )
					overlap_minimum( column( _areas, c ), *_cols[c], orientation::HORIZONTAL );

				for ( size_t r = 0; r < _rows.size(); ++r )
					overlap_minimum( _areas[r], *_rows[r], orientation::VERTICAL );

				flow_minimum( _cols, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );
				flow_minimum( _rows, master, direction::DOWN, _vspacing, _pad[2], _pad[3] );
				break;
		}
	}

	virtual void recompute_layout( container &master )
	{
		switch ( _dir )
		{
			case direction::LEFT:
				/*
				xflow_constraint( _areas, _weights, master, _dir, _hspacing, _pad[0], _pad[1] );
				overlap_constraint( _areas, master, orientation::VERTICAL, _pad[2], _pad[3] );
				*/
				break;

			case direction::RIGHT:
				/*
				xflow_constraint( _areas, _weights, master, _dir, _hspacing, _pad[1], _pad[0] );
				overlap_constraint( _areas, master, orientation::VERTICAL, _pad[2], _pad[3] );
				*/
				break;

			case direction::UP:
				/*
				xflow_constraint( _areas, _weights, master, _dir, _vspacing, _pad[2], _pad[3] );
				overlap_constraint( _areas, master, orientation::HORIZONTAL, _pad[0], _pad[1] );
				*/
				break;

			case direction::DOWN:
			{
				std::array<double,2> cweights {{ 0.0, 1.0 }};

				flow_constraint( _rows, _weights, master, direction::DOWN, _vspacing, _pad[2], _pad[3] );
				flow_constraint( _cols, cweights, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );

				for ( size_t c = 0; c < _cols.size(); ++c )
					overlap_constraint( column( _areas, c ), *_cols[c], orientation::HORIZONTAL );

				for ( size_t r = 0; r < _rows.size(); ++r )
					overlap_constraint( _areas[r], *_rows[r], orientation::VERTICAL );
				break;
			}
		}
	}

private:
	typedef std::array<std::shared_ptr<area>,2> row;

	direction _dir = direction::DOWN;
	double _pad[4] = { 0.0, 0.0, 0.0, 0.0 };
	double _hspacing = 0.0, _vspacing = 0.0;
	std::vector<row> _areas;
	std::vector<double> _weights;

	std::vector<std::shared_ptr<simple_area>> _rows;
	std::vector<std::shared_ptr<simple_area>> _cols;
};

////////////////////////////////////////

}

