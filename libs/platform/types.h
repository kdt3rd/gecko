//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/point.h>
#include <base/size.h>
#include <base/rect.h>

////////////////////////////////////////

namespace platform
{

// could potentially use a 16-bit integer which might be useful if we
// ever run on a low-end SoC...
using coord_type = int;
using point = base::point<coord_type>;
using size = base::size<coord_type>;
using rect = base::rect<coord_type>;

} // namespace platform



