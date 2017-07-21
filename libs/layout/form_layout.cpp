//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "form_layout.h"
#include "label_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

form_layout::form_layout( const std::shared_ptr<area> &labels )
	: _labels( labels )
{
	precondition( labels, "null labels area" );
}

////////////////////////////////////////

std::shared_ptr<area> form_layout::add( const std::shared_ptr<area> &entry, const std::shared_ptr<area> &label )
{
	auto result = std::make_shared<label_layout>( label, entry );
	_areas.push_back( entry );
	return result;
}

////////////////////////////////////////

void form_layout::compute_bounds( void )
{
	// Clean up areas that have been deleted.
	_areas.remove_if( []( const std::weak_ptr<area> &a ) { return a.expired(); } );

	_labels->compute_bounds();

	double minw = 0.0;
	double maxw = 0.0;

	for ( auto &w: _areas )
	{
		auto a = w.lock();
		if ( a )
		{
			minw = std::max( minw, a->minimum_width() );
			maxw = std::max( maxw, a->maximum_width() );
		}
	}
	_min_entry_w = minw;

	minw += _labels->minimum_width() + _pad[0] + _pad[1] + _spacing[0];
	maxw += _labels->minimum_width() + _pad[0] + _pad[1] + _spacing[0];

	double minh = _labels->minimum_height() + _pad[2] + _pad[3];
	double maxh = _labels->maximum_height() + _pad[2] + _pad[3];

	set_minimum( minw, minh );
	set_maximum( maxw, maxh );
}

////////////////////////////////////////

void form_layout::compute_layout( void )
{
	double lw = width() - _pad[0] - _pad[1] - _spacing[0] - _min_entry_w;
	double h = height() - _pad[2] - _pad[3];

	_labels->set( { _pad[0], _pad[2] }, { lw, h } );
	std::cout << "Labels " << *_labels << std::endl;
	_labels->compute_layout();

	double ex = width() - _pad[1] - _min_entry_w;
	for ( auto &w: _areas )
	{
		auto a = w.lock();
		if ( a )
		{
			a->set_x( ex );
			a->set_width( _min_entry_w );
			a->compute_layout();
			std::cout << "Entry: " << *a << std::endl;
		}
	}
}

////////////////////////////////////////

}

