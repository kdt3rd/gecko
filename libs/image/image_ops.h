//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "image.h"
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

image_buf combine( const plane &p );

image_buf combine( const plane &p, const plane &p2 );

image_buf combine( const plane &p, const plane &p2, const plane &p3 );

image_buf combine( const plane &p, const plane &p2, const plane &p3, const plane &p4 );

image_buf union_dim( const image_buf &a, const image_buf &b, float outside = 0.F );
image_buf union_dim_hold( const image_buf &a, const image_buf &b );

} // namespace image



