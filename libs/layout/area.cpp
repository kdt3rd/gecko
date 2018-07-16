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

void area::set_expansion_flex( coord f )
{
	precondition( f >= min_coord(), "invalid expansion flex" );
	_flex = f;
}

////////////////////////////////////////

coord area::expand_width( coord w, coord overall_flex )
{
	precondition( overall_flex >= min_coord(), "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	coord used = min_coord();
	if ( w > min_coord() )
	{
		coord maxflex = divide( w * _flex, overall_flex );
		used = std::min( std::max( min_coord(), _max.w() - width() ), maxflex );
		set_width( width() + used );
	}

	return used;
}

////////////////////////////////////////

coord area::expand_height( coord h, coord overall_flex )
{
	precondition( overall_flex >= min_coord(), "invalid overall flex {0}", overall_flex );
	precondition( _flex <= overall_flex, "invalid overall flex ({0}/{1})", _flex, overall_flex );

	coord used = min_coord();
	if ( h > min_coord() )
	{
		coord maxflex = divide( h * _flex, overall_flex );
		used = std::min( std::max( min_coord(), _max.h() - height() ), maxflex );
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

