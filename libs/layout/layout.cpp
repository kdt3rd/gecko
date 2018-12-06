//
// Copyright (c) 2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "layout.h"

namespace layout
{

////////////////////////////////////////

layout::~layout( void )
{
}

////////////////////////////////////////

void layout::expand_width( std::list<std::shared_ptr<area>> &lst, coord extra )
{
	if ( extra <= min_coord() )
		return;

	// Sort by priority (descending order).
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > min_coord() )
	{
		coord overall_flex = min_coord();
		int32_t pri = lst.front()->expansion_priority();
		while ( !lst.empty() && lst.front()->expansion_priority() == pri )
		{
			overall_flex += lst.front()->expansion_flex();
			work.splice( work.end(), lst, lst.begin() );
		}

		if ( overall_flex > min_coord() )
		{
			coord used = min_coord();
			do
			{
				for ( auto &a: work )
					used += a->expand_width( extra, overall_flex );
				extra -= used;
			} while ( used > min_coord() && extra > min_coord() );
		}
		else
		{
			if ( !work.empty() )
			{
				coord grow = divide( extra, work.size() );
				for ( auto &a: work )
					a->set_width( a->width() + grow );
				extra = min_coord();
			}
		}
		work.clear();
	}
}

////////////////////////////////////////

void layout::expand_height( std::list<std::shared_ptr<area>> &lst, coord extra )
{
	if ( extra <= min_coord() )
		return;

	// Sort by priority
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > min_coord() )
	{
		coord overall_flex = min_coord();
		int32_t pri = lst.front()->expansion_priority();
		while ( !lst.empty() && lst.front()->expansion_priority() == pri )
		{
			overall_flex += lst.front()->expansion_flex();
			work.splice( work.end(), lst, lst.begin() );
		}

		if ( overall_flex > min_coord() )
		{
			coord used = min_coord();
			do
			{
				for ( auto &a: work )
					used += a->expand_height( extra, overall_flex );
				extra -= used;
			} while ( used > min_coord() && extra > min_coord() );
		}
		else
		{
			if ( !work.empty() )
			{
				coord grow = divide( extra, work.size() );
				for ( auto &a: work )
					a->set_height( a->height() + grow );
				extra = min_coord();
			}
		}
		work.clear();
	}
}

////////////////////////////////////////

}

