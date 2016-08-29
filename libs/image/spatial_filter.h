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

#include "image.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

/// applies a morphological erode, returning the min
plane erode( const plane &p, int radius );

/// applies a morphological dilate, returning the max of the area
plane dilate( const plane &p, int radius );

/// NB: diameter, not radius, so 3 is a 3x3 median
plane median( const plane &p, int diameter );

/// returns the median of 3 things: median of the 5 pixels of a cross,
/// the median of the 5 pixels of an x, and the incoming plane itself
plane cross_x_img_median( const plane &p );

/// returns the median value from three planes
plane median3( const plane &p1, const plane &p2, const plane &p3 );

plane despeckle( const plane &p, float thresh );

/// dx and dy are radius in x and y, sigD is the sigma for distance, sigI is the sigma for image distance
plane bilateral( const plane &p1, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );
plane cross_bilateral( const plane &p1, const plane &ref, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );
plane weighted_bilateral( const plane &p1, const plane &weight, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );

/// Implements one form of undecimated wavelet filter
plane wavelet_filter( const plane &p, size_t levels, float sigma );
plane wavelet_filter( const plane &p, size_t levels, const plane &sigma );

plane guided_filter_mono( const plane &I, const plane &p, int r, float eps );
plane guided_filter_mono( const plane &I, const plane &p, int r, const plane &eps, float epsScale = 1.F );

image_buf guided_filter_mono( const image_buf &I, const image_buf &p, int r, float eps );
image_buf guided_filter_mono( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale = 1.F );

image_buf guided_filter_color( const image_buf &I, const image_buf &p, int r, float eps );
image_buf guided_filter_color( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale = 1.F );

plane savitsky_golay_filter( const plane &p, int radius, int order );
plane savitsky_golay_minimize_error( const plane &p, int radius, int max_order );

void add_spatial( engine::registry &r );

} // namespace image

