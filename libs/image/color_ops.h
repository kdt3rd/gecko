// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "image.h"

namespace color
{
class state;
}

////////////////////////////////////////

namespace image
{
image_buf colorspace(
    const image_buf &a, const color::state &from, const color::state &to );

void add_color_ops( engine::registry &r );

} // namespace image
