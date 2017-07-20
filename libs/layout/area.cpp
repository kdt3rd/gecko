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

void area::set_expansion_flex( double f )
{
	precondition( f >= 0.0, "invalid expansion flex" );
	_flex = f;
}

////////////////////////////////////////

double area::expand_width( double w, double overall_flex )
{
	precondition( overall_flex >= 0.0, "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	double used = 0.0;
	if ( w > 0.0 )
	{
		double maxflex = w * _flex / overall_flex;
		used = std::min( _max.w() - width(), maxflex );
		set_width( width() + used );
	}

	return used;
}

////////////////////////////////////////

double area::expand_height( double h, double overall_flex )
{
	precondition( overall_flex >= 0.0, "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	double used = 0.0;
	if ( h > 0.0 )
	{
		double maxflex = h * _flex / overall_flex;
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

