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
#include <base/alignment.h>
#include <layout/types.h>

////////////////////////////////////////

namespace gui
{

//using coord = double;
using coord = layout::coord;
using rect = base::rect<coord>;
using point = rect::point_type;
using size = rect::size_type;

// we don't collect in the GUI library, so we can just use this, right?
template<typename Func> using signal = base::signal<Func>;

using alignment = base::alignment;

} // namespace gui



