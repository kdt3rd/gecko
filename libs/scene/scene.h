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

	/// This is equivalent to calling
	///   find_or_create( root(), name, provider, type, version )
	///
	/// @sa find_or_create   
	inline node_reference find_or_create(
		const char *name,
		const char *provider,
		const char *type,
		const char *version )
	{
		return find_or_create( root(), name, provider, type, version );
	}

	/// provider might be a name of a plugin
	///
	/// type is the type of node to create
	///
	/// version is the version of the object last saved (may be NULL
	/// to get latest)
	///
	/// subname is the location name to create underneath parent. This
	/// can still be a tree path (a/b/c), which is considered relative
	/// to the parent.  if any intermediate levels do not exist, this
	/// will fail
	///
	/// This is safe to do regardless of whether the provider / type /
	/// version actually exists. This allows a script to be loaded in
	/// a context where a plugin may not correctly exist
	///
	/// if the parent location is not writable (meaning some other
	/// thread is currently doing something with the parent, this may
	/// block until the other thread releases ownership)
	node_reference find_or_create(
		const node_reference &parent,
		const char *subname,
		const char *provider,
		const char *type,
		const char *version );

	void reparent( const node_reference &n, const node_reference &newparent );
	void remove( const node_reference &r, bool reparent_children );

private:
	std::shared_ptr<node> _root;
};

} // namespace scene

