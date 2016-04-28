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



