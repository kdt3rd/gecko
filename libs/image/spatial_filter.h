// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

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

plane despeckle( const plane &p, float removeBright = 1.F, float removeDark = 1.F );

/// dx and dy are radius in x and y, sigD is the sigma for distance, sigI is the sigma for image distance
plane bilateral( const plane &p1, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );
plane cross_bilateral( const plane &p1, const plane &ref, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );
plane weighted_bilateral( const plane &p1, const plane &weight, const engine::computed_value<int> &dx, const engine::computed_value<int> &dy, const engine::computed_value<float> &sigD, const engine::computed_value<float> &sigI );

/// Implements one form of undecimated wavelet filter
plane wavelet_filter( const plane &p, int levels, float sigma );
plane wavelet_filter( const plane &p, int levels, const plane &sigma );

plane guided_filter_mono( const plane &I, const plane &p, int r, float eps );
plane guided_filter_mono( const plane &I, const plane &p, int r, const plane &eps, float epsScale = 1.F );

image_buf guided_filter_mono( const image_buf &I, const image_buf &p, int r, float eps );
image_buf guided_filter_mono( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale = 1.F );

image_buf guided_filter_color( const image_buf &I, const image_buf &p, int r, float eps );
image_buf guided_filter_color( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale = 1.F );

plane savitsky_golay_filter( const plane &p, int radius, int order );
plane savitsky_golay_minimize_error( const plane &p, int radius, int max_order );

plane nlm( const plane &p, int search, int compare, float searchSigma, float compareSigma, float centerWeight );
plane nlm( const plane &p, int search, int compare, float searchSigma, const plane &compareSigma, float centerWeight );
plane nlm( const std::vector<plane> &p, int search, int compare, float searchSigma, float compareSigma, float centerWeight );
image_buf nlm( const image_buf &p, int search, int compare, float searchSigma, float compareSigma, float centerWeight );
image_buf nlm( const std::vector<image_buf> &p, int search, int compare, float searchSigma, float compareSigma, float centerWeight );

/// use L-1 norm instead of L-2 (faster because we can precompute
/// areas for differences)
plane nlm_L1( const plane &p, int search, int compare, float searchSigma, float compareSigma, float centerWeight );

void add_spatial( engine::registry &r );

} // namespace image

