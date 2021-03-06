// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "image.h"
#include "vector_field.h"

////////////////////////////////////////

namespace image
{
vector_field concatenate( const vector_field &a, const vector_field &b );
vector_field project( const vector_field &a, float scale );

plane confidence(
    const vector_field &a, const vector_field &b, int conservativeness = 2 );

image_buf warp_dirac( const image_buf &src, const vector_field &v );
plane     warp_dirac( const plane &src, const vector_field &v );

image_buf warp_bilinear( const image_buf &src, const vector_field &v );
plane     warp_bilinear( const plane &src, const vector_field &v );

image_buf warp_lanczos( const image_buf &src, const vector_field &v );
plane     warp_lanczos( const plane &src, const vector_field &v );

vector_field convert_to_absolute( const vector_field &v );
vector_field convert_to_relative( const vector_field &v );

/// This currently is based on the method used in the KITTI evaluation
/// database. It is normalized based on the vector magnitude, with an
/// optional scale. The direction and magnitude are converted to HSV
/// notation, and then converted to RGB.
///
/// The extension above KITTI is to additionally produce the vector
/// magnitude as the 4th (alpha) channel
image_buf
colorize( const vector_field &v, const plane &alpha, float scale = 8.F );

vector_field modify_vectors( const plane &newu, const plane &newv, bool isAbs );

void add_vector_ops( engine::registry &r );

} // namespace image
