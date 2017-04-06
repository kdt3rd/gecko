//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <image/scanline.h>

////////////////////////////////////////

namespace image
{

namespace sse4
{

void div_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );

void div_numberplane( scanline &dest, float v, const scanline &src );

} // namespace sse4

} // namespace image


