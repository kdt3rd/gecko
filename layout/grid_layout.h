
#pragma once

#include <memory>
#include "layout.h"
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Grid layout.
///
/// Lays out areas in a grid of rows and columns.
template<typename container, typename area>
class grid_layout : public layout<container, area>
{
public:
	grid_layout( void )
	{
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

	void add_row( const std::vector<std::shared_ptr<area>> &add, double w = 0.0 )
	{
		if ( _rows == 0 && _cols == 0 )
		{
			_cols = add.size();
			for ( size_t i = 0; i < _cols; ++i )
			{
				_careas.push_back( std::make_shared<simple_area>() );
				_cweights.push_back( 0.0 );
			}
		}

		precondition( add.size() == _cols, "invalid row size" );
		_rareas.push_back( std::make_shared<simple_area>() );
		_rweights.push_back( w );
		_rows++;
		_areas.push_back( add );

		for ( auto &a: add )
			this->added( a );
	}

	void add_column( const std::vector<std::shared_ptr<area>> &add, double w = 0.0 )
	{
		if ( _rows == 0 && _cols == 0 )
		{
			_rows = add.size();
			for ( size_t i = 0; i < _rows; ++i )
			{
				_rareas.push_back( std::make_shared<simple_area>() );
				_rweights.push_back( 0.0 );
			}
		}

		precondition( add.size() == _rows, "invalid column size" );
		_careas.push_back( std::make_shared<simple_area>() );
		_cweights.push_back( w );
		_cols++;

		for ( size_t i = 0; i < _areas.size(); ++i )
		{
			_areas[i].push_back( add[i] );
			this->added( add[i] );
		}
	}

	virtual void recompute_minimum( container &master )
	{
		for ( auto &r: _areas )
		{
			for ( auto &a: r )
				a->layout();
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
				for ( size_t c = 0; c < _careas.size(); ++c )
					overlap_minimum( column( _areas, c ), *_careas[c], orientation::HORIZONTAL );

				for ( size_t r = 0; r < _rareas.size(); ++r )
					overlap_minimum( _areas[r], *_rareas[r], orientation::VERTICAL );

				flow_minimum( _careas, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );
				flow_minimum( _rareas, master, direction::UP, _vspacing, _pad[2], _pad[3] );
				break;

			case direction::DOWN:
				for ( size_t c = 0; c < _careas.size(); ++c )
					overlap_minimum( column( _areas, c ), *_careas[c], orientation::HORIZONTAL );

				for ( size_t r = 0; r < _rareas.size(); ++r )
					overlap_minimum( _areas[r], *_rareas[r], orientation::VERTICAL );
				flow_minimum( _careas, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );
				flow_minimum( _rareas, master, direction::DOWN, _vspacing, _pad[2], _pad[3] );
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
			{
				flow_constraint( _careas, _cweights, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );
				for ( size_t c = 0; c < _careas.size(); ++c )
					overlap_constraint( column( _areas, c ), *_careas[c], orientation::HORIZONTAL );

				flow_constraint( _rareas, _rweights, master, direction::UP, _vspacing, _pad[2], _pad[3] );
				for ( size_t r = 0; r < _rareas.size(); ++r )
					overlap_constraint( _areas[r], *_rareas[r], orientation::VERTICAL );
				break;
			}

			case direction::DOWN:
			{
				flow_constraint( _careas, _cweights, master, direction::RIGHT, _hspacing, _pad[0], _pad[1] );
				for ( size_t c = 0; c < _careas.size(); ++c )
					overlap_constraint( column( _areas, c ), *_careas[c], orientation::HORIZONTAL );

				flow_constraint( _rareas, _rweights, master, direction::DOWN, _vspacing, _pad[2], _pad[3] );
				for ( size_t r = 0; r < _rareas.size(); ++r )
					overlap_constraint( _areas[r], *_rareas[r], orientation::VERTICAL );

				break;
			}
		}
	}

private:
	typedef std::vector<std::shared_ptr<area>> row;

	direction _dir = direction::DOWN;
	double _pad[4] = { 0.0, 0.0, 0.0, 0.0 };
	double _hspacing = 0.0, _vspacing = 0.0;

	std::vector<row> _areas;

	std::vector<std::shared_ptr<simple_area>> _rareas;
	std::vector<double> _rweights;

	std::vector<std::shared_ptr<simple_area>> _careas;
	std::vector<double> _cweights;

	size_t _rows = 0, _cols = 0;
};

////////////////////////////////////////

}

