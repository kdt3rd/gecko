//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <image/plane.h>
#include <image/accum_buf.h>

////////////////////////////////////////

namespace image
{

namespace sse3
{

accum_buf compute_SAT( const plane &p, int power );

} // namespace sse3

} // namespace image



