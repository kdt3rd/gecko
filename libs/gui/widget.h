//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>
#include <layout/area.h>
#include <platform/keyboard.h>
#include <platform/context.h>
#include <platform/event.h>
#include <base/contract.h>
#include "context.h"
#include "widget_ptr.h"

namespace gui
{

using event = platform::event;

////////////////////////////////////////

class widget : public rect, public std::enable_shared_from_this<widget>
{
public:
	widget( void );
	explicit widget( std::unique_ptr<layout::area> &&a );
	virtual ~widget( void );

	/// Handle changes to the monitor configuration (refresh rate,
	/// output color space, etc.)
	virtual void monitor_changed( context &ctxt );
	virtual void build( context &ctxt );
	virtual void paint( context &ctxt );

	virtual bool mouse_press( const point &p, int button );
	virtual bool mouse_release( const point &p, int button );
	virtual bool mouse_move( const point &p );
	virtual bool mouse_wheel( int a );
	virtual bool key_press( platform::scancode c );
	virtual bool key_release( platform::scancode c );
	virtual bool text_input( char32_t c );

	const std::shared_ptr<layout::area> &layout_target( void ) const
	{
		return _area;
	}

	virtual bool update_layout( double duration );

protected:
	void invalidate( const rect &r )
	{
		context::current().invalidate( r );
	}

	void invalidate( void )
	{
		context::current().invalidate( *this );
	}

private:
	std::shared_ptr<layout::area> _area;
	rect _anim_start;
	double _anim_time;
};

////////////////////////////////////////

}

