//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "label_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

label_layout::label_layout( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e )
	: _label( l ), _entry( e )
{
}

////////////////////////////////////////

void label_layout::compute_bounds( void )
{
	auto l = _label.lock();
	auto e = _entry.lock();

	base::size min;
	base::size max;

	if ( l )
	{
		min = l->minimum_size();
		max = l->maximum_size();
	}
	if ( e )
		min.set_height( std::max( min.h(), e->minimum_height() ) );

	set_minimum( min );
	set_maximum( max );
}

////////////////////////////////////////

void label_layout::compute_layout( void )
{
	auto e = _entry.lock();

	if ( e )
	{
		e->set_y( y1() );
		e->set_height( height() );
	}
}

////////////////////////////////////////

}

