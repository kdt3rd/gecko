//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
			_ranges.erase( _ranges.begin() + i );
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



