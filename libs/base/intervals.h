
#pragma once

#include "range.h"
#include "reverse.h"
#include <algorithm>
#include <vector>

namespace base
{

////////////////////////////////////////

template<typename Number>
class intervals
{
private:
	std::vector<range<Number>> _ranges;

public:
	intervals( void )
	{
	}

	size_t size( void ) const
	{
		return _ranges.size();
	}

	auto begin( void ) -> decltype( _ranges.begin() )
	{
		return _ranges.begin();
	}

	auto end( void ) -> decltype( _ranges.end() )
	{
		return _ranges. end();
	}

	template<typename ...Types>
	void insert( Number x, Types ...rest )
	{
		_ranges.emplace_back( x, x );
		insert( rest... );
	}

private:
	void insert( void )
	{
		merge();
	}

	void merge( void )
	{
		std::sort( _ranges.begin(), _ranges.end() );
		for ( size_t i = 1; i < _ranges.size(); ++i )
		{
			auto &r1 = _ranges[i-1];
			auto &r2 = _ranges[i];

			if ( r2.follows( r1 ) )
			{
				r1.extend( r2 );
				_ranges.erase( _ranges.begin() + long(i) );
				--i;
			}
		}
	}
};

////////////////////////////////////////

}

