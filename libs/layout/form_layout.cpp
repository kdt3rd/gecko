//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "form_layout.h"
#include "field_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

form_layout::form_layout( const std::shared_ptr<area> &layout )
	: _layout( layout )
{
	precondition( layout, "null layout" );
}

////////////////////////////////////////

void form_layout::add( const std::shared_ptr<field_layout> &f )
{
	_fields.push_back( f );
}

////////////////////////////////////////

void form_layout::compute_bounds( void )
{
	// Compute the size of the fields
	double minw = 0.0;
	for ( auto &f: _fields )
		minw = std::max( minw, f->field_minimum_width() );

	// Set the (minimum) size of the fields
	for ( auto &f: _fields )
		f->set_field_width( minw );

	// Now compute the sub-layout bounds.
	_layout->compute_bounds();

	base::size pads( _pad[0] + _pad[1], _pad[2] + _pad[3] );
	set_minimum( _layout->minimum_size() + pads );
	set_maximum( _layout->maximum_size() + pads );
}

////////////////////////////////////////

void form_layout::compute_layout( void )
{
	_layout->set( { x1() + _pad[0], y1() + _pad[2] }, { width() - _pad[0] - _pad[1], height() - _pad[2] - _pad[3] } );
	_layout->compute_layout();
}

////////////////////////////////////////

}

