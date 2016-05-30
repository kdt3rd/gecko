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

#include "types.h"
#include <vector>

////////////////////////////////////////

namespace engine
{

class subgroup;
class node;

///
/// @brief Class subgroup_function provides...
///
class subgroup_function
{
public:
	subgroup_function( void );
	subgroup_function( const subgroup_function & ) = delete;
	subgroup_function( subgroup_function && ) = delete;
	subgroup_function&operator=( const subgroup_function & ) = delete;
	subgroup_function&operator=( subgroup_function && ) = delete;
	virtual ~subgroup_function( void );

	virtual void update_inputs( int d ) = 0;

	inline bool is_output( void ) const;
	inline size_t output_index( void ) const;
	inline void output_index( size_t i );

	virtual void bind( std::vector<std::shared_ptr<subgroup_function>> &, subgroup &, node & ) = 0;

	virtual void deref_inputs( void ) = 0;

	inline const std::vector< std::pair<subgroup_function *, size_t> > &outputs( void ) const;

	void add_output( subgroup_function *p, size_t idx );

protected:
	std::vector<std::pair<subgroup_function *, size_t>> _outputs;
	size_t _out_idx = size_t(-1);
};

////////////////////////////////////////

inline bool
subgroup_function::is_output( void ) const
{
	return _out_idx != size_t(-1);
}

////////////////////////////////////////

inline size_t
subgroup_function::output_index( void ) const
{
	return _out_idx;
}

////////////////////////////////////////

inline void subgroup_function::output_index( size_t i )
{
	_out_idx = i;
}

////////////////////////////////////////

inline const std::vector< std::pair<subgroup_function *, size_t> > &
subgroup_function::outputs( void ) const
{
	return _outputs;
}

} // namespace engine



