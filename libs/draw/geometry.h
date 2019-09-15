// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "polyline.h"

namespace draw
{
////////////////////////////////////////

size_t circle_precision( dim r );

void add_quadratic(
    const polyline::point &p1,
    const polyline::point &p2,
    const polyline::point &p3,
    polyline &             line );
void add_cubic(
    const polyline::point &p1,
    const polyline::point &p2,
    const polyline::point &p3,
    const polyline::point &p4,
    polyline &             line );
void add_arc(
    const polyline::point &center,
    dim                    radius,
    float                  a1,
    float                  a2,
    polyline &             line );

////////////////////////////////////////

} // namespace draw
