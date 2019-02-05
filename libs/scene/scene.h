// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "node_reference.h"
#include <memory>

////////////////////////////////////////

namespace scene
{

class light_hierarchy;

class scene
{
public:
	node_reference root( void ) const;
	light_hierarchy &lights( void ) const;

	/// assumes the names of the levels are separated by '/'
	node_reference find( uint64_t id ) const;
	node_reference find( const char *name ) const { return root()->find( name ); }

	node_reference find_or_create( const char *name, const char *type );

	void reparent( const node_reference &n, const node_reference &newparent );
	void remove( const node_reference &r, bool reparent_children );

private:
	std::shared_ptr<node> _root;
};

} // namespace scene

