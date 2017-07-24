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
	double minw = 0.0;
	for ( auto &f: _fields )
		minw = std::max( minw, f->field_minimum_width() );

	for ( auto &f: _fields )
		f->set_field_width( minw );

	_layout->compute_bounds();

	set_minimum( _layout->minimum_size() );
	set_maximum( _layout->maximum_size() );
}

////////////////////////////////////////

void form_layout::compute_layout( void )
{
	_layout->set( { x1(), y1() }, { width(), height() } );
	_layout->compute_layout();
}

////////////////////////////////////////

}

