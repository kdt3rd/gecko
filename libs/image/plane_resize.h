//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
/// would result in a plane that is < max(2, minSize) pixels wide or high (so the
/// minimum size is 2x2)
///
std::vector<plane> make_pyramid( const plane &in, const std::string &filter, float eta, int n = 0, int minSize = 2 );
std::vector<image_buf> make_pyramid( const image_buf &in, const std::string &filter, float eta, int n = 0, int minSize = 2 );

void add_resize( engine::registry &r );

} // namespace image



