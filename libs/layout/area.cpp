//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "area.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

area::~area( void )
{
}

////////////////////////////////////////

void area::set_expansion_flex( coord_type f )
{
	precondition( f >= 0.0, "invalid expansion flex" );
	_flex = f;
}

////////////////////////////////////////

area::coord_type area::expand_width( coord_type w, coord_type overall_flex )
{
	precondition( overall_flex >= 0.0, "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	coord_type used = 0.0;
	if ( w > 0.0 )
	{
		coord_type maxflex = w * _flex / overall_flex;
		used = std::min( _max.w() - width(), maxflex );
		set_width( width() + used );
	}

	return used;
}

////////////////////////////////////////

area::coord_type area::expand_height( coord_type h, coord_type overall_flex )
{
	precondition( overall_flex >= 0.0, "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	coord_type used = 0.0;
	if ( h > 0.0 )
	{
		coord_type maxflex = h * _flex / overall_flex;
		used = std::min( _max.h() - height(), maxflex );
		set_height( height() + used );
	}
	return used;
}

////////////////////////////////////////

void area::compute_bounds( void )
{
}

////////////////////////////////////////

void area::compute_layout( void )
{
}

////////////////////////////////////////

}

