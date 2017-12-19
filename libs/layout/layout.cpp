
#include "layout.h"

namespace layout
{

////////////////////////////////////////

layout::~layout( void )
{
}

////////////////////////////////////////

void layout::expand_width( std::list<std::shared_ptr<area>> &lst, double extra )
{
	if ( extra <= 0.0 )
		return;

	// Sort by priority (descending order).
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > 0.0 )
	{
		double overall_flex = 0.0;
		int32_t pri = lst.front()->expansion_priority();
		while ( !lst.empty() && lst.front()->expansion_priority() == pri )
		{
			overall_flex += lst.front()->expansion_flex();
			work.splice( work.end(), lst, lst.begin() );
		}

		if ( overall_flex > 0.0 )
		{
			double used = 0.0;
			do
			{
				for ( auto &a: work )
					used += a->expand_width( extra, overall_flex );
				extra -= used;
			} while ( used > 0.0 && extra > 0.0 );
		}
		else
		{
			if ( !work.empty() )
			{
				double grow = extra / work.size();
				for ( auto &a: work )
					a->set_width( a->width() + grow );
				extra = 0.0;
			}
		}
		work.clear();
	}
}

////////////////////////////////////////

void layout::expand_height( std::list<std::shared_ptr<area>> &lst, double extra )
{
	if ( extra <= 0.0 )
		return;

	// Sort by priority
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > 0.0 )
	{
		double overall_flex = 0.0;
		int32_t pri = lst.front()->expansion_priority();
		while ( !lst.empty() && lst.front()->expansion_priority() == pri )
		{
			overall_flex += lst.front()->expansion_flex();
			work.splice( work.end(), lst, lst.begin() );
		}

		if ( overall_flex > 0.0 )
		{
			double used = 0.0;
			do
			{
				for ( auto &a: work )
					used += a->expand_height( extra, overall_flex );
				extra -= used;
			} while ( used > 0.0 && extra > 0.0 );
		}
		else
		{
			if ( !work.empty() )
			{
				double grow = extra / work.size();
				for ( auto &a: work )
					a->set_height( a->height() + grow );
				extra = 0.0;
			}
		}
		work.clear();
	}
}

////////////////////////////////////////

}

