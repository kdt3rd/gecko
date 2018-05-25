//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>
#include <layout/area.h>
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

	virtual std::shared_ptr<widget> find_widget_under( coord x, coord y );

	virtual bool mouse_press( const event &e );
	virtual bool mouse_release( const event &e );
	virtual bool mouse_move( const event &e );
	virtual bool mouse_wheel( const event &e );

	virtual bool key_press( const event &e );
	virtual bool key_repeat( const event &e );
	virtual bool key_release( const event &e );

	virtual bool text_input( const event &e );

	// TBD: do we need to split these into multiple functions as we
	// did with mouse / keyboard?
	virtual bool tablet_event( const event &e );
	virtual bool hid_event( const event &e );
	virtual bool user_event( const event &e );

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

