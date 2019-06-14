// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

// include this first to make stream operator visible to format
#include <base/spooky_hash.h>

#include <base/any.h>

#include <cstddef>
#include <cstdint>
//#include <experimental/any>
#include <ostream>

////////////////////////////////////////

namespace engine
{
typedef base::spooky_hash hash;
inline std::ostream &     operator<<( std::ostream &os, const hash::value &v )
{
    return base::operator<<( os, v );
}

using any = base::any;
using base::any_cast;

/// Storage type for storing the dimensions of a particular processing node
///
/// storing the bytes necessary for storage, as well as 6 dimensions that can be used to store the data window for a plane, the number of planes and the number of images
///
/// TODO: do we need anything different for audio processing? is int16_t enough for 1D things???
struct dimensions
{
    constexpr dimensions( void ) {}
    typedef int16_t value_type;
    value_type      x1             = 0;
    value_type      y1             = 0;
    value_type      x2             = 0;
    value_type      y2             = 0;
    value_type      planes         = 0;
    value_type      images         = 0;
    value_type      bytes_per_item = 0;
    value_type      reserved       = 0;
};
constexpr dimensions nulldim;
inline hash &        operator<<( hash &h, const dimensions &d )
{
    h << d.x1 << d.y1 << d.x2 << d.y2 << d.planes << d.images
      << d.bytes_per_item;
    return h;
}
inline bool operator==( const dimensions &a, const dimensions &b )
{
    return (
        a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2 &&
        a.planes == b.planes && a.images == b.images &&
        a.bytes_per_item == b.bytes_per_item );
}
inline bool operator!=( const dimensions &a, const dimensions &b )
{
    return !( a == b );
}
std::ostream &operator<<( std::ostream &os, const dimensions &d );

/// @brief storage type for storing an operator id
///
/// 65536 operations should be plenty, right?
typedef uint16_t op_id;
constexpr op_id  nullop = op_id( -1 );

/// @brief storage type for storing a node id
///
/// There should be able to be hundreds of thousands of nodes, so
/// 16-bit is not sufficient
typedef uint32_t  node_id;
constexpr node_id nullnode = node_id( -1 );

} // namespace engine
