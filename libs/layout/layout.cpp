
#include "layout.h"

namespace layout
{

////////////////////////////////////////

void layout::expand_width( const std::list<std::weak_ptr<area>> &areas, double extra )
{
	if ( extra <= 0.0 )
		return;

	// Lock the pointers and remove any null pointer.
	std::list<std::shared_ptr<area>> lst;
	for ( auto &p: areas )
	{
		auto a = p.lock();
		if ( a )
			lst.push_back( a );
	}

	// Sort by priority (descending order).
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > 0.0 )
	{
		double overall_flex = 0.0;
		size_t pri = lst.front()->expansion_priority();
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
		work.clear();
	}
}

////////////////////////////////////////

void layout::expand_height( const std::list<std::weak_ptr<area>> &areas, double extra )
{
	if ( extra <= 0.0 )
		return;

	// Lock the pointers and remove any null pointer.
	std::list<std::shared_ptr<area>> lst;
	for ( auto &p: areas )
	{
		auto a = p.lock();
		if ( a )
			lst.push_back( a );
	}

	// Sort by priority
	lst.sort( []( const std::shared_ptr<area> &a, const std::shared_ptr<area> &b ) { return b->expansion_priority() < a->expansion_priority(); } );

	std::list<std::shared_ptr<area>> work;
	while ( !lst.empty() && extra > 0.0 )
	{
		double overall_flex = 0.0;
		size_t pri = lst.front()->expansion_priority();
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
		work.clear();
	}
}

////////////////////////////////////////

}

