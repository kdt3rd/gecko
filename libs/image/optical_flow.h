//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "vector_field.h"
#include "vector_ops.h"
#include "patch_match.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

//// TODO: Add arguments
// with patch match, do we even need this?
//vector_field oflow_sad( const plane &a, const plane &b );

/// @brief Optical Flow based on Anisotropic Huber-L1 Optical Flow
/// from Werlberger / Trobin / Pock / Wedel / Cremers / Bischof
/// with modifications
/// 
/// lambda ~40-100 usually, range is 10 - 1000
/// theta 0.1, range 0.1 - 0.5 - controls quadratic relaxation
/// epsilon 0.005, 0.1 - tv norm w/ quadratic behavior between 0 and eps
/// edgePower
/// edgeAlpha turns on edge based processing
/// innerIters 10, controls tv-l1 iters per warp
/// warpIters 5, controls number of warps per pyramid
/// eta pyramid scale factor 0.5, 0.5 - 0.95, 0.5 is fastest
vector_field oflow_ahtvl1( const plane &a, const plane &b, const plane &alpha = plane(), const plane &alphaNext = plane(), const vector_field &initUV = vector_field(), float lambda = 70.F, float theta = 0.1F, float epsilon = 0.005F, float edgePower = 2.F, float edgeAlpha = 50.F, int edgeBorder = 5, int tvl1Iters = 100, int warpIters = 2, bool adaptiveIters = true, float eta = 0.5F );

//// TODO: Add arguments and implement
//vector_field oflow_htvl1( const image_buf &a, const image_buf &b );

/// @brief Optical Flow method derived from Werlberger / Pock & Chambolle
///
/// Both Werlberger CVPR 2010 and Pock & Chambolle's Primal Dual paper from May 2010
///
/// lambda ~40-100 usually, range is 10 - 1000, best if controlled by grain amp. estimate
/// gamma 0.001 - 0.1 seems useful, controls weighting of color change component
/// innerIters 20 - XXX, controls refinement iters per warp
/// warpIters 1-5, controls number of warps per pyramid, subdivides innerIters based on level
/// scaleFactor 0.5, 0.5 - 0.95, 0.5 is fastest
/// alpha - when alpha is 0, vector field should be 0,0
/// initUV - initial guess at u,v vector field
vector_field oflow_primaldual( const plane &a, const plane &b, const plane &alpha = plane(), const plane &alphaNext = plane(), const vector_field &initUV = vector_field(), float lambda = 40.F, float gamma = 0.001F, int innerIters = 200, int warpIters = 5, bool adaptiveIters = true, float eta = 0.5F );
//// TODO: Add arguments and implement
//vector_field oflow_primaldual( const image_buf &a, const image_buf &b );

vector_field oflow_zilch_alpha( const vector_field &a, const plane &alpha );

void add_oflow( engine::registry &r );

} // namespace image

