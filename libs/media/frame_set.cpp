//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "frame_set.h"
#include <algorithm>

////////////////////////////////////////

namespace media
{

////////////////////////////////////////

frame_set::~frame_set( void )
{
}

////////////////////////////////////////

void
frame_set::add( int64_t f )
{
	_ranges.push_back( std::make_pair( f, f ) );
	// std::pair comparison operator is fine
	std::sort( _ranges.begin(), _ranges.end() );
	size_t i = 1;
	while ( i < _ranges.size() )
	{
		std::pair<int64_t, int64_t> &prior = _ranges[i-1];
		std::pair<int64_t, int64_t> &cur = _ranges[i];
		if ( ( prior.second + 1 ) == cur.first )
		{
			prior.second = cur.second;
			_ranges.erase( _ranges.begin() + static_cast<ssize_t>( i ) );
		}
		else
		{
			++i;
		}
	}
	++_count;
}

////////////////////////////////////////

bool
frame_set::contains( int64_t f ) const
{
	for ( auto &r: _ranges )
	{
		if ( r.first <= f && r.second >= f )
			return true;
	}
	return false;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &os, const frame_set &fs )
{
	for ( size_t i = 0; i < fs.ranges(); ++i )
	{
		auto r = fs.range( i );
		if ( i > 0 )
			os << ',';
		os << r.first << '-' << r.second;
	}
	return os;
}

////////////////////////////////////////

} // media
