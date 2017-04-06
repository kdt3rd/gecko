//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once
#include "plane.h"

////////////////////////////////////////

namespace image
{

bool check_nan( const plane &p );
void check_nan_and_report( const plane &p );

} // namespace image



