//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/contract.h>
#include "plane.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

/// [-1, 1]
plane instant_gradient_horiz( const plane &p );
/// [-1, 1]'
plane instant_gradient_vert( const plane &p );
/// [-1, 0, 1] / 2
plane central_gradient_horiz( const plane &p );
/// [-1, 0, 1]' / 2
plane central_gradient_vert( const plane &p );
/// OLD: [1, -8, 0, 8, -1] / 12
/// NEW: [-1, -2, 0, 2, 1] / 8
plane noise_gradient_horiz5( const plane &p );
/// [1, -8, 0, 8, -1]' / 12
plane noise_gradient_vert5( const plane &p );
/// alpha-aware versions of the above, which when a pixel is in the
/// alpha, the gradient becomes zero, if the pixel is on the boundary,
/// it's treated as the edge of the picture (held), else as above
plane instant_gradient_horiz( const plane &p, const plane &alpha );
plane instant_gradient_vert( const plane &p, const plane &alpha );
plane central_gradient_horiz( const plane &p, const plane &alpha );
plane central_gradient_vert( const plane &p, const plane &alpha );
plane noise_gradient_horiz5( const plane &p, const plane &alpha );
plane noise_gradient_vert5( const plane &p, const plane &alpha );

plane convolve_horiz( const plane &p, const std::vector<float> &k );
plane convolve_vert( const plane &p, const std::vector<float> &k );

inline plane separable_convolve( const plane &p, const std::vector<float> &k )
{
	// do the vert pass first so we get n:1, 1:1 group behavior
	return convolve_horiz( convolve_vert( p, k ), k );
}

void add_convolve( engine::registry &r );

} // namespace image



