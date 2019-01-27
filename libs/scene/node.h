// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "time.h"
#include "evaluation_context.h"

////////////////////////////////////////

namespace scene
{

class node
{
public:
	/// will receive a default object id for later addressing
	node( const char *n );
	node( const char *n, uint64_t override_id );
	~node( void );

	uint64_t id( void ) const { return _id; }
	const char *name( void ) const { return _name; }

	/// assumes the names of the levels are separated by '/'
	node *find( const char *n ) const;

	transform_stack &get_transform_stack( const time &ctxt );
	matrix &model_to_world( const time &ctxt ) const;

	void expand( const evaluation_context &ctxt );

	size_t child_count( void ) const;
	node &child( size_t i ) const;
	node &operator[]( size_t i ) const;

private:
	uint64_t _id;
	const char *_name;
};

} // namespace scene

