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

#include "scanline_group.h"

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

scanline_group::scanline_group( int w, size_t nOuts )
	: _outputs( nOuts, scanline() ), _scan_width( w )
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
scanline_group::find_or_checkout( const std::vector<scanline> &inputs )
{
	for ( auto &s: inputs )
	{
		if ( s.unique() )
			return s;
	}

	if ( _outputs.size() == 1 )
	{
		if ( _outputs[0].unique() )
			return _outputs[0];
	}

	for ( auto &s: _spare )
	{
		if ( ! s.unique() )
			return s;
	}

	_spare.emplace_back( scanline( _scan_width ) );
	return _spare.back();
}

} // image



