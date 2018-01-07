//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>

////////////////////////////////////////

namespace color
{

///
/// @brief Class triplet provides a simple collection of 3 values
///
template <typename V>
class triplet
{
	static_assert( std::is_floating_point<V>::value, "color::triplet requires floating point type" );
public:
	typedef V value_type;

	inline constexpr triplet( void ) noexcept : x(value_type(0)), y(value_type(0)), z(value_type(0)) {}
	inline constexpr triplet( value_type v ) : x(v), y(v), z(v) {}
	inline constexpr triplet( value_type a, value_type b, value_type c ) : x(a), y(b), z(c) {}

	value_type x, y, z;
};

} // namespace color



