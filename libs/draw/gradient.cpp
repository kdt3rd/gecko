
#include "gradient.h"
#include <cmath>
#include <algorithm>

////////////////////////////////////////

namespace draw
{

////////////////////////////////////////

void gradient::sort( void )
{
	auto cmp = []( const std::pair<float,gl::color> &a, const std::pair<float,gl::color> &b )
	{
		return a.first < b.first;
	};
	std::sort( _stops.begin(), _stops.end(), cmp );
}

////////////////////////////////////////

gl::color gradient::sample( float v ) const
{
	size_t i = 0;
	while ( i < _stops.size() )
	{
		if ( _stops[i].first > v )
			break;
		++i;
	}

	if ( i < _stops.size() )
	{
		if ( i == 0 )
			return _stops[0].second;

		float range = _stops[i].first - _stops[i-1].first;
		float mix = ( _stops[i].first - v ) / range;

		return gl::color::mix( _stops[i].second, _stops[i-1].second, mix );
	}

	return _stops.back().second;
}

////////////////////////////////////////

}
