//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "overlap_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

overlap::overlap( void )
{
}

////////////////////////////////////////

void overlap::compute_bounds( void )
{
	// Clean up areas that have been deleted.
	_areas.remove_if( []( const std::weak_ptr<area> &a ) { return a.expired(); } );

	double minw = 0.0;
	double minh = 0.0;
	double maxw = 0.0;
	double maxh = 0.0;

	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
		{
			a->compute_bounds();
			minw = std::max( minw, a->minimum_width() );
			minh = std::max( minh, a->minimum_height() );
			maxw = std::max( maxw, a->maximum_width() );
			maxh = std::max( maxh, a->maximum_height() );
		}
	}

	minw += _pad[0] + _pad[1];
	minh += _pad[2] + _pad[3];
	maxw += _pad[0] + _pad[1];
	maxh += _pad[2] + _pad[3];

	if ( _areas.empty() )
	{
		maxw = 1e6;
		maxh = 1e6;
	}

	set_minimum( minw, minh );
	set_maximum( maxw, maxh );
}

////////////////////////////////////////

void overlap::compute_layout( void )
{
	double x = this->x() + _pad[0];
	double y = this->y() + _pad[2];
	double w = width() - _pad[0] - _pad[1];
	double h = height() - _pad[2] - _pad[3];

	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
		{
			a->set_position( { x, y } );
			a->set_size( w, h );
			a->compute_layout();
		}
	}
}

////////////////////////////////////////

}

