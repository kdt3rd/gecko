// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "subgroup_function.h"

////////////////////////////////////////

namespace engine
{
////////////////////////////////////////

subgroup_function::subgroup_function( void ) {}

////////////////////////////////////////

subgroup_function::~subgroup_function( void ) {}

////////////////////////////////////////

void subgroup_function::add_output( subgroup_function *p, size_t idx )
{
    _outputs.emplace_back( std::make_pair( p, idx ) );
}

////////////////////////////////////////

} // namespace engine
