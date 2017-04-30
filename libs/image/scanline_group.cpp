//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "scanline_group.h"
#include <iostream>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

scanline_group::scanline_group( int offx, int w, size_t nOuts )
	: _outputs( nOuts, scanline() ), _scan_offset( offx ), _scan_width( w )
{
}

////////////////////////////////////////

scanline_group::~scanline_group( void )
{
}

////////////////////////////////////////

void
scanline_group::output_scan( size_t i, scanline &&s )
{
	_outputs[i] = std::move( s );
}

////////////////////////////////////////

scanline
scanline_group::output_scan_and_clear( size_t i )
{
	scanline r;
	swap( r, _outputs[i] );
	return r;
}

////////////////////////////////////////

scanline
scanline_group::find_or_checkout( const std::vector<scanline> &inputs, bool in_place )
{
	if ( in_place )
	{
		for ( auto &s: inputs )
		{
			if ( s.unique() )
			{
//			std::cout << "reusing input scanline as destinaton" << std::endl;
				return s;
			}
		}
		

		if ( _outputs.size() == 1 )
		{
			if ( _outputs[0].unique() )
			{
//			std::cout << "using output scanline as destinaton" << std::endl;
				return _outputs[0];
			}
		}
	}

	for ( auto &s: _spare )
	{
		if ( s.unique() )
		{
//			std::cout << "using existing spare as destinaton" << std::endl;
			return s;
		}
	}

//	std::cout << "creating new spare as destinaton" << std::endl;
	_spare.emplace_back( scanline( _scan_offset, _scan_width ) );
	return _spare.back();
}

} // image



