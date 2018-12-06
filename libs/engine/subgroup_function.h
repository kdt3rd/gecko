//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
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



