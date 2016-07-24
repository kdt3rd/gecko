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

#include "vector_field.h"
#include "image.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

enum class patch_style : int
{
	SSD, ///< just sum square difference of values
	SSD_GRAD, ///< sum square difference of values and gradients
	SSD_GRAD_DIST, ///< sum square difference of values and gradients as well as a distance penalizer
	GRAD ///< sum square difference gradients only
};

vector_field patch_match( const plane &a, const plane &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters );
vector_field patch_match( const image_buf &a, const image_buf &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters );

vector_field hier_patch_match( const plane &a, const plane &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters );
vector_field hier_patch_match( const image_buf &a, const image_buf &b, int64_t framenumA, int64_t framenumB, int radius, patch_style style, int iters );

void add_patchmatch( engine::registry &r );

} // namespace image

