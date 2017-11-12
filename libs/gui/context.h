//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/scope_guard.h>
#include <base/rect.h>
#include "style.h"

namespace gui
{

////////////////////////////////////////

/// @brief Context for gui.
class context
{
public:
	context( void ) = default;
	virtual ~context( void );
	context( const context & ) = delete;
	context( context && ) = delete;
	context &operator=( const context & ) = delete;
	context &operator=( context && ) = delete;

	virtual void invalidate( const base::rect &r ) = 0;

	const style &get_style( void ) const
	{
		return _style;
	}

	style &get_style( void )
	{
		return _style;
	}

	template<typename func>
	void in_context( func &&f )
	{
		push_context();
		on_scope_exit { pop_context(); };
		std::forward<func>(f)();
	}

	static context &current( void );

protected:
	void push_context( void );
	void pop_context( void );

	style _style;
};

////////////////////////////////////////

}

