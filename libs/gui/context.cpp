// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include "context.h"
#include <base/contract.h>

namespace
{
std::vector<gui::context *> _contexts;
}

namespace gui
{

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

void context::push_context( void )
{
	_contexts.push_back( this );
}

////////////////////////////////////////

void context::pop_context( void )
{
	precondition( !_contexts.empty(), "context not available" );
	precondition( _contexts.back() == this, "incorrect context" );
	_contexts.pop_back();
}

////////////////////////////////////////

context &context::current( void )
{
	precondition( !_contexts.empty(), "context not available" );
	return *_contexts.back();
}

////////////////////////////////////////

platform::context::clip_region_guard
context::push_clip( const rect &r )
{
	return hw_context().push_clip( to_native( r ) );
}

////////////////////////////////////////

}
