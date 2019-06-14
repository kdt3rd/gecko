// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "image.h"
#include "op_registry.h"
#include "vector_field.h"

////////////////////////////////////////

namespace image
{
enum class patch_style : int
{
    SSD,           ///< just sum square difference of values
    SSD_GRAD,      ///< sum square difference of values and gradients
    SSD_GRAD_DIST, ///< sum square difference of values and gradients as well as a distance penalizer
    GRAD           ///< sum square difference gradients only
};

vector_field patch_match(
    const plane &a,
    const plane &b,
    const plane &alpha,
    int64_t      framenumA,
    int64_t      framenumB,
    int          radius,
    patch_style  style,
    int          iters );
/// pass in empty plane for no-alpha treatment
vector_field patch_match(
    const image_buf &a,
    const image_buf &b,
    const plane &    alpha,
    int64_t          framenumA,
    int64_t          framenumB,
    int              radius,
    patch_style      style,
    int              iters );

vector_field hier_patch_match(
    const plane &a,
    const plane &b,
    const plane &alpha,
    int64_t      framenumA,
    int64_t      framenumB,
    int          radius,
    patch_style  style,
    int          iters );
/// pass in empty plane for no-alpha treatment
vector_field hier_patch_match(
    const image_buf &a,
    const image_buf &b,
    const plane &    alpha,
    int64_t          framenumA,
    int64_t          framenumB,
    int              radius,
    patch_style      style,
    int              iters );

void add_patchmatch( engine::registry &r );

} // namespace image
