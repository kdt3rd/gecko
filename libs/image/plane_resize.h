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

#include "plane.h"
#include "image.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

plane resize_horiz_point( const plane &p, int neww );
plane resize_vert_point( const plane &p, int newh );

inline plane resize_point( const plane &p, int neww, int newh )
{
	return resize_horiz_point( resize_vert_point( p, newh ), neww );
}

plane resize_horiz_bilinear( const plane &p, int neww );
plane resize_vert_bilinear( const plane &p, int newh );

inline plane resize_bilinear( const plane &p, int neww, int newh )
{
	return resize_horiz_bilinear( resize_vert_bilinear( p, newh ), neww );
}

plane resize_horiz_bicubic( const plane &p, int neww );
plane resize_vert_bicubic( const plane &p, int newh );

inline plane resize_bicubic( const plane &p, int neww, int newh )
{
	return resize_horiz_bicubic( resize_vert_bicubic( p, newh ), neww );
}

plane resize_horiz( const plane &p, const std::string &filter, int neww );
plane resize_vert( const plane &p, const std::string &filter, int newh );

inline plane resize( const plane &p, const std::string &filter, int neww, int newh )
{
	return resize_horiz( resize_vert( p, filter, newh ), filter, neww );
}

////////////////////////////////////////

/// Makes a pyramid, where the returned vector has in as the first
/// element, and the last element is the coarsest scale.
///
/// The eta is used to compute the scale factor, so the resulting images should be of size
///
/// eta^0, eta^1, eta^2, ..., eta^n
///
/// if n == 0, this progresses until the resulting the next level
/// would result in a plane that is < 2 pixels wide or high (so the
/// minimum size is 2x2)
///
std::vector<plane> make_pyramid( const plane &in, const std::string &filter, float eta, int n = 0 );
std::vector<image_buf> make_pyramid( const image_buf &in, const std::string &filter, float eta, int n = 0 );

void add_resize( engine::registry &r );

} // namespace image



