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

#pragma once

#include "scanline.h"
#include <vector>

////////////////////////////////////////

namespace image
{

///
/// @brief Class scanline_group provides...
///
class scanline_group
{
public:
	explicit scanline_group( int w, size_t nOuts );
	~scanline_group( void );

	void output_scan( size_t i, scanline &&s );
	scanline output_scan_and_clear( size_t i );

	// if the number of outputs is > 1, this will never return an output scanline
	scanline find_or_checkout( const std::vector<scanline> &inputs );

private:
	std::vector<scanline> _outputs;
	std::vector<scanline> _spare;
	int _scan_width = 0;
};

} // namespace image



