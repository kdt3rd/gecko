//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <cstdint>
#include <cstddef>
#include <base/spooky_hash.h>
//#include <experimental/any>
#include <base/any.h>
#include <ostream>

////////////////////////////////////////

namespace engine
{

typedef base::spooky_hash hash;
inline std::ostream &operator<<( std::ostream &os, const hash::value &v )
{
	return base::operator<<( os, v );
}

using any = base::any;
template <typename T>
T any_cast( const any &a )
{
	return a.as<T>();
}
template <typename T>
T any_cast( any &a )
{
	return a.as<T>();
}

/// Storage type for storing the dimensions of a particular processing node
///
/// storing 4 values so that there can be constructs such as a list of
/// images represented - x width, y height, z planes, w images
struct dimensions
{
	typedef uint16_t value_type;
	value_type x = 0, y = 0, z = 0, w = 0;
};
constexpr dimensions nulldim = { 0, 0, 0, 0 };
inline hash &operator<<( hash &h, const dimensions &d )
{
	h << d.x << d.y << d.z << d.w;
	return h;
}
inline bool operator==( const dimensions &a, const dimensions &b )
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
inline bool operator!=( const dimensions &a, const dimensions &b )
{
	return !( a == b );
}
std::ostream &operator<<( std::ostream &os, const dimensions &d );

/// @brief storage type for storing an operator id
///
/// 65536 operations sounds like plenty...
typedef uint16_t op_id;
constexpr op_id nullop = op_id(-1);

/// @brief storage type for storing a node id
///
/// There should be able to be hundreds of thousands of nodes, so
/// 16-bit is not sufficient
typedef uint32_t node_id;
constexpr node_id nullnode = node_id(-1);

} // namespace engine



