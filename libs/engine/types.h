//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
///
/// TODO: we use an int32_t - is this enough for audio applications?
/// more than enough for imaging given the multiple dimensions
struct dimensions
{
	constexpr dimensions( void ) {}
	typedef int32_t value_type;
	value_type x = 0, y = 0, z = 0, w = 0;
};
constexpr dimensions nulldim;
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
/// 65536 operations should be plenty, right?
typedef uint16_t op_id;
constexpr op_id nullop = op_id(-1);

/// @brief storage type for storing a node id
///
/// There should be able to be hundreds of thousands of nodes, so
/// 16-bit is not sufficient
typedef uint32_t node_id;
constexpr node_id nullnode = node_id(-1);

} // namespace engine



