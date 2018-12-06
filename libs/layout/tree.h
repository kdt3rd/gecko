//
// Copyright (c) 2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include "layout.h"

namespace layout
{

////////////////////////////////////////

class tree : public layout
{
public:
	tree( void )
	{
	}

	tree( const std::shared_ptr<area> &g, const std::shared_ptr<area> &t )
		: _groove( g ), _title( t )
	{
	}

	tree( coord i, const std::shared_ptr<area> &t )
		: _title( t ), _indent( i )
	{
	}

	void set_groove( const std::shared_ptr<area> &g )
	{
		_groove = g;
	}

	void set_title( const std::shared_ptr<area> &t )
	{
		_title = t;
	}

	void set_indent( coord i )
	{
		_indent = i;
	}

	void add( const std::shared_ptr<area> &c )
	{
		_areas.push_back( c );
	}

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::weak_ptr<area> _groove;
	std::weak_ptr<area> _title;
	std::list<std::weak_ptr<area>> _areas;
	coord _indent = coord(10);
};

////////////////////////////////////////

}

