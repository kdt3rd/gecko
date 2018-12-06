//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#include "context.h"
#include <base/contract.h>

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

context::context( void )
{
}

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

void
context::push_clip_rect( const rect &r )
{
	_cur_clip_stack.push( r );
	reset_clip( r );
}

////////////////////////////////////////

void
context::pop_clip_rect( void )
{
	precondition( ! _cur_clip_stack.empty(), "clip stack not empty" );
	_cur_clip_stack.pop();
	if ( _cur_clip_stack.empty() )
		reset_clip( rect() );
	else
		reset_clip( _cur_clip_stack.top() );
}

////////////////////////////////////////

} // platform



