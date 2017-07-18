//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "box_layout.h"

namespace layout
{

////////////////////////////////////////

hbox_layout::hbox_layout( void )
{
}

////////////////////////////////////////

void hbox_layout::compute_minimum( void )
{
	double w = 0.0;
	double h = 0.0;
	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
		{
			a->compute_minimum();
			w += _spacing[0] + a->minimum_width();
			h = std::max( h, a->minimum_height() );
		}
	}

	set_minimum( w + _pad[0] + _pad[1], h + _pad[2] + _pad[3] );
}

////////////////////////////////////////

void hbox_layout::compute_layout( void )
{
	_areas.remove_if( []( const std::weak_ptr<area> &a ) { return a.expired(); } );

	double x = _pad[0];
	double y = _pad[2];
	double h = std::max( 0.0, height() - _pad[2] - _pad[3] );

	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
		{
			double aw = a->minimum_width();
			a->set( { x, y }, { aw, h } );
			x += aw + _spacing[0];
			a->compute_layout();
		}
	}
}

////////////////////////////////////////

}

