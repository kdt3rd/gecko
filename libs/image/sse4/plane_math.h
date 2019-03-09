// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

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


