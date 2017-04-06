//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "subgroup_function.h"

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

subgroup_function::subgroup_function( void )
{
}

////////////////////////////////////////

subgroup_function::~subgroup_function( void )
{
}

////////////////////////////////////////

void
subgroup_function::add_output( subgroup_function *p, size_t idx )
{
	_outputs.emplace_back( std::make_pair( p, idx ) );
}

////////////////////////////////////////

} // engine



