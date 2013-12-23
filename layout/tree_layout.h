
#pragma once

#include <memory>
#include "layout.h"
#include "constraint.h"

namespace layout
{

////////////////////////////////////////

/// @brief Tree layout.
///
/// Organizes areas in single row (or column).
/// Each area can have a weight.  Extra space will be distributed among the areas by weight.
template<typename container, typename area>
class tree_layout : public layout<container,area>
{
public:
	tree_layout( double tabsize = 12.0, direction dir = direction::DOWN )
		: _tab_size( tabsize ), _dir( dir )
	{
	}

	void set_root( const std::shared_ptr<area> &r )
	{
		_root = r;
		this->added( r );
	}

	void set_tab( const std::shared_ptr<area> &t )
	{
		_tab = t;
		this->added( t );
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

	void add( const std::shared_ptr<area> &a, double weight = 0.0 )
	{
		_areas.push_back( a );
		_weights.push_back( weight );
		this->added( a );
	}

	virtual void recompute_minimum( container &master )
	{
		simple_area children;
		double minw, minh;

		switch ( _dir )
		{
			case direction::LEFT:
				flow_minimum( _areas, children, _dir, _hspacing );
				overlap_minimum( _areas, children, orientation::VERTICAL );
				minw = _root->minimum_width() + children.minimum_width() + _hspacing;
	   			minh = std::max( _root->minimum_height(), children.minimum_height() + _tab_size );
				if ( _tab )
					_tab->set_minimum( 0.0, _tab_size );
				break;

			case direction::RIGHT:
				flow_minimum( _areas, children, _dir, _hspacing );
				overlap_minimum( _areas, children, orientation::VERTICAL );
				minw = _root->minimum_width() + children.minimum_width() + _hspacing;
	   			minh = std::max( _root->minimum_height(), children.minimum_height() + _tab_size );
				if ( _tab )
					_tab->set_minimum( 0.0, _tab_size );
				break;

			case direction::UP:
				flow_minimum( _areas, children, _dir, _vspacing );
				overlap_minimum( _areas, children, orientation::HORIZONTAL );
	   			minw = std::max( _root->minimum_width(), children.minimum_width() + _tab_size );
				minh = _root->minimum_height() + children.minimum_height() + _vspacing;
				if ( _tab )
					_tab->set_minimum( _tab_size, 0.0 );
				break;

			case direction::DOWN:
				flow_minimum( _areas, children, _dir, _vspacing );
				overlap_minimum( _areas, children, orientation::HORIZONTAL );
	   			minw = std::max( _root->minimum_width(), children.minimum_width() + _tab_size );
				minh = _root->minimum_height() + children.minimum_height() + _vspacing;
				if ( _tab )
					_tab->set_minimum( _tab_size, 0.0 );
				break;
		}

		master.set_minimum( minw + _pad[0] + _pad[1], minh + _pad[2] + _pad[3] );
	}

	virtual void recompute_layout( container &master )
	{
		simple_area children = master;
		children.shrink( _pad[0], _pad[1], _pad[2], _pad[3] );

		switch ( _dir )
		{
			case direction::LEFT:
				_root->set_position( children.bottom_left() );
				_root->set_size( children.width(), -_root->minimum_height() );
				children.shrink( _tab_size, 0, _root->height(), 0 );
				flow_constraint( _areas, _weights, master, _dir, _hspacing );
				overlap_constraint( _areas, master, orientation::VERTICAL );
				break;

			case direction::RIGHT:
				_root->set_position( children.bottom_left() );
				_root->set_size( children.width(), -_root->minimum_height() );
				children.shrink( _tab_size, 0, _root->height(), 0 );
				flow_constraint( _areas, _weights, master, _dir, _hspacing );
				overlap_constraint( _areas, master, orientation::VERTICAL );
				break;

			case direction::UP:
				_root->set_position( children.bottom_left() );
				_root->set_size( children.width(), -_root->minimum_height() );
				children.shrink( _tab_size, 0, 0, _root->height() + _vspacing );
				flow_constraint( _areas, _weights, children, _dir, _vspacing );
				overlap_constraint( _areas, children, orientation::HORIZONTAL );
				if ( _tab )
				{
					_tab->set_position( children.top_left() );
					_tab->set_size( -_tab_size, 0.0 );
					if ( !_areas.empty() )
						_tab->set_vertical( _areas.back()->y1(), _root->y1() );
				}
				break;

			case direction::DOWN:
				_root->set_position( children.top_left() );
				_root->set_size( children.width(), _root->minimum_height() );
				children.shrink( _tab_size, 0, _root->height() + _vspacing, 0 );
				flow_constraint( _areas, _weights, children, _dir, _vspacing );
				overlap_constraint( _areas, children, orientation::HORIZONTAL );
				if ( _tab )
				{
					_tab->set_position( children.top_left() );
					_tab->set_size( -_tab_size, 0.0 );
					if ( !_areas.empty() )
						_tab->set_vertical( _root->y2(), _areas.back()->y2() );
				}
				break;
		}
	}

protected:
	double _tab_size;
	direction _dir = direction::DOWN;
	double _pad[4] = { 0.0, 0.0, 0.0, 0.0 };
	double _hspacing = 0.0, _vspacing = 0.0;
	std::shared_ptr<area> _root;
	std::shared_ptr<area> _tab;
	std::vector<std::shared_ptr<area>> _areas;
	std::vector<double> _weights;
};

////////////////////////////////////////

}

