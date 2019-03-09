// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "form.h"
#include "field.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

form::form( const std::shared_ptr<area> &layout )
	: _layout( layout )
{
	precondition( layout, "null layout" );
}

////////////////////////////////////////

void form::add( const std::shared_ptr<field> &f )
{
	_fields.push_back( f );
}

////////////////////////////////////////

void form::compute_bounds( void )
{
	// Compute the size of the fields
	coord minw = min_coord();
	for ( auto &f: _fields )
		minw = std::max( minw, f->field_minimum_width() );

	// Set the (minimum) size of the fields
	for ( auto &f: _fields )
		f->set_field_width( minw );

	// Now compute the sub-layout bounds.
	_layout->compute_bounds();

	size pads( _pad[0] + _pad[1], _pad[2] + _pad[3] );
	set_minimum( _layout->minimum_size() + pads );
	set_maximum( _layout->maximum_size() + pads );
}

////////////////////////////////////////

void form::compute_layout( void )
{
	_layout->set( { x1() + _pad[0], y1() + _pad[2] }, { width() - _pad[0] - _pad[1], height() - _pad[2] - _pad[3] } );
	_layout->compute_layout();
}

////////////////////////////////////////

}

