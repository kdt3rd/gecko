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

	void add_dependency( const node &n );
	transform_stack &get_transform_stack( const time &ctxt );
	matrix &model_to_world( const time &ctxt ) const;

	void process( const evaluation_context &ctxt );

protected:
	virtual void evaluate( const evaluation_context &ctxt );

private:
	uint64_t _id;
	const char *_name;
};

} // namespace scene

