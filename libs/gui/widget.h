// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "context.h"
#include "event.h"
#include "widget_ptr.h"

#include <base/contract.h>
#include <layout/area.h>
#include <platform/context.h>
#include <platform/event.h>
#include <type_traits>

namespace gui
{
////////////////////////////////////////

class widget
    : public rect
    , public std::enable_shared_from_this<widget>
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

    virtual std::shared_ptr<widget> find_widget_under( const point &p );

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
    void invalidate( const rect &r ) { context::current().invalidate( r ); }

    void invalidate( void ) { context::current().invalidate( *this ); }

private:
    std::shared_ptr<layout::area> _area;
    rect                          _anim_start;
    double                        _anim_time = -1.0;
};

////////////////////////////////////////

} // namespace gui
