//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <limits>
#include <base/units.h>
#include <base/rect.h>
#include <base/alignment.h>

////////////////////////////////////////

namespace layout
{

//using coord_type = double;
//constexpr inline coord_type divide( coord_type a, coord_type b )
//{ return a / b; }

//using coord = int32_t;
//constexpr inline coord divide( coord a, coord b )
//{ return ( a + ( b - 1 ) ) / b; }
using coord = base::units::millimeters<float>;
template <typename U>
constexpr inline coord divide( const coord &a, U && b )
{ return a / coord( std::forward<U>( b ) ); }

using rect = base::rect<coord>;
using point = rect::point_type;
using size = rect::size_type;

inline constexpr coord min_coord( void ) { return coord(0); }
inline constexpr coord max_coord( void ) { return std::numeric_limits<coord>::max(); }

using alignment = base::alignment;

}
