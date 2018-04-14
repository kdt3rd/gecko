//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <cstddef>

#include <base/point.h>
#include <base/size.h>
#include <base/rect.h>

////////////////////////////////////////

namespace platform
{

// Lowest common denominator in real life so far is int16 in the X
// protocol for drawing rectangles, etc
using coord_type = int16_t;
//using size_type = uint16_t;

using point = base::point<coord_type>;
using size = base::size<coord_type>;
using rect = base::rect<coord_type>;

} // namespace platform



