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
#include <base/signal.h>


////////////////////////////////////////

namespace gui
{

// do we need double precision?
using coord_type = double;
using point = base::point<coord_type>;
using size = base::size<coord_type>;
using rect = base::rect<coord_type>;

// we don't collect here, right?
template<typename Func> using signal = base::signal<Func>;

} // namespace gui



