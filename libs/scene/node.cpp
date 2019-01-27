// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "node.h"

////////////////////////////////////////

namespace scene
{

////////////////////////////////////////

node::node( const char *n )
		: _name( n )
{
}

////////////////////////////////////////

node::~node( void )
{
}

////////////////////////////////////////

node *node::find( const char *n ) const
{
	size_t l = strlen( _name );
	return 0 == strncmp( _name, n, l ) && ( n[l] == '\0' || n[l] == '/' );
}

////////////////////////////////////////

} // namespace scene

