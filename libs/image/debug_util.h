// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once
#include "plane.h"

////////////////////////////////////////

namespace image
{
bool check_nan( const plane &p );
void check_nan_and_report( const plane &p );

} // namespace image
