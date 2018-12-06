//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include <draw/types.h>
#include <layout/types.h>
#include <base/rect.h>
#include <base/signal.h>
#include <base/alignment.h>

////////////////////////////////////////

namespace gui
{

using color = draw::color;

//using coord = double;
using coord = layout::coord;
using rect = base::rect<coord>;
using point = rect::point_type;
using size = rect::size_type;

// we don't collect in the GUI library, so we can just use this, right?
template<typename Func> using signal = base::signal<Func>;

using alignment = base::alignment;

} // namespace gui
