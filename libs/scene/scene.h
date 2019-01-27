// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

class scene
{
public:
	node &root( void ) const;
	light_hierarchy &lights( void ) const;

	/// assumes the names of the levels are separated by '/'
	node *find( const char *name ) const { return root().find( name ); }
};

} // namespace scene

