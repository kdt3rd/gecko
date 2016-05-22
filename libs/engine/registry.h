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

#include <base/const_string.h>
#include "types.h"
#include <vector>
#include "op.h"

////////////////////////////////////////

namespace engine
{

///
/// @brief Class registry provides...
///
class registry
{
public:
	registry( void );
	~registry( void );

	op_id find( const base::cstring &n ) const;
	op_id find_constant( const std::type_info &ti ) const;

	op_id add( const op &o );
	op_id add( op &&o );
	template <typename T>
	void register_constant( void );

	inline const op &get( op_id i ) const;
	inline const op &operator[]( op_id i ) const;

	/// registry for "pod" types + std::string,
	/// for use with computed_value
	static const registry &pod_registry( void );
private:
	std::vector<op> _ops;
	std::map<std::string, op_id> _name_to_op;
};

////////////////////////////////////////

template <typename T>
void registry::register_constant( void )
{
	// name is not guaranteed to be unique, but neither is hash_code,
	// but should be good enough for the expected types
	const std::type_info &ti = typeid(T);
	add( op( ti.name(), ti, op::value ) );
}


////////////////////////////////////////

inline const op &registry::get( op_id i ) const
{
	return _ops[i];
}

inline const op &registry::operator[]( op_id i ) const
{
	return get( i );
}


} // namespace engine



