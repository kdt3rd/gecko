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


