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

#include <base/contract.h>
#include "plane.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

/// [-1, 0, 1] / 2
plane central_gradient_horiz( const plane &p );
/// [-1, 0, 1]' / 2
plane central_gradient_vert( const plane &p );

plane convolve_horiz( const plane &p, const std::vector<float> &k );
plane convolve_vert( const plane &p, const std::vector<float> &k );

inline plane separable_convolve( const plane &p, const std::vector<float> &k )
{
	// do the vert pass first so we get n:1, 1:1 group behavior
	return convolve_horiz( convolve_vert( p, k ), k );
}

/// NB: diameter, not radius, so 3 is a 3x3 median
plane median( const plane &p, int diameter );

/// returns the median of 3 things: median of the 5 pixels of a cross,
/// the median of the 5 pixels of an x, and the incoming plane itself
plane cross_x_img_median( const plane &p );

/// returns the median value from three planes
plane median3( const plane &p1, const plane &p2, const plane &p3 );

void add_plane_area( engine::registry &r );

} // namespace image



