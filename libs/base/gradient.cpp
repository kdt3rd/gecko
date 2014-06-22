
#include "gradient.h"
#include <cmath>

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

color gradient::sample( double v ) const
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

		double range = _stops[i].first - _stops[i-1].first;
		double mix = ( _stops[i].first - v ) / range;

		return color::mix( _stops[i].second, _stops[i-1].second, mix );
	}

	return _stops.back().second;
}

////////////////////////////////////////

}
