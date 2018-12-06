//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <image/scanline.h>

////////////////////////////////////////

namespace image
{

namespace sse3
{

void assign_value( scanline &dest, float v );
void add_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
void add_planenumber( scanline &dest, const scanline &srcA, float v );

void sub_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );

void mul_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
void mul_planenumber( scanline &dest, const scanline &srcA, float v );

void div_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );

void div_numberplane( scanline &dest, float v, const scanline &src );

void muladd_planeplaneplane( scanline &dest, const scanline &srcA, const scanline &srcB, const scanline &srcC );

void muladd_planenumbernumber( scanline &dest, const scanline &src, float a, float b );

} // namespace sse3

} // namespace image


