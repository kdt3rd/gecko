// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "checkbox.h"

#include "application.h"

#include <draw/icons.h>
#include <draw/paint.h>
#include <draw/path.h>
#include <iostream>

namespace gui
{
////////////////////////////////////////

checkbox_w::checkbox_w( void ) {}

////////////////////////////////////////

checkbox_w::~checkbox_w( void ) {}

////////////////////////////////////////

void checkbox_w::build( context &ctxt )
{
    const style &s = ctxt.get_style();

    draw::paint        c;
    platform::context &hwc = ctxt.hw_context();
    gl::api &          ogl = hwc.api();

    _unchecked.rebuild( hwc );
    _checked.rebuild( hwc );

    c.set_fill_color( s.active_icon( s.background_color() ) );
    _unchecked.add( ogl, draw::iconCheckBoxEmpty(), c );

    c.set_fill_color( s.dominant_color() );
    _checked.add( ogl, draw::iconCheckBoxChecked(), c );

    size sz     = s.widget_minimum_size();
    auto native = ctxt.to_native( sz );
    _unchecked.shape_size( native.w(), native.h() );
    _checked.shape_size( native.w(), native.h() );
    _unchecked.set_size( sz.w(), sz.h() );
    _checked.set_size( sz.w(), sz.h() );

    auto &l = layout_target();
    l->set_minimum( sz );
    l->set_maximum( sz );
}

////////////////////////////////////////

void checkbox_w::paint( context &ctxt )
{
    _checked.set_position( x(), y() );
    _unchecked.set_position( x(), y() );
    bool c = _state;
    if ( _tracking )
        c = _current;
    if ( c )
        _checked.draw( ctxt.hw_context() );
    else
        _unchecked.draw( ctxt.hw_context() );
}

////////////////////////////////////////

bool checkbox_w::mouse_press( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    context::current().grab_source( e, shared_from_this() );

    _tracking = true;
    _current  = !_state;
    invalidate();

    return _tracking;
}

////////////////////////////////////////

bool checkbox_w::mouse_release( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    if ( _tracking )
    {
        _tracking = false;
        if ( contains( e.from_native( e.raw_mouse().x, e.raw_mouse().y ) ) )
            set_state( _current );
        _current = _state;
        invalidate();
        context::current().release_source( e );
        return true;
    }

    return false;
}

////////////////////////////////////////

bool checkbox_w::mouse_move( const event &e )
{
    if ( _tracking )
    {
        if ( contains( e.from_native( e.raw_mouse().x, e.raw_mouse().y ) ) )
            _current = !_state;
        else
            _current = _state;
        invalidate();
        return true;
    }
    return false;
}

////////////////////////////////////////

void checkbox_w::set_state( bool s )
{
    if ( _state != s )
    {
        _state = s;
        when_toggled( _state );
    }
}
////////////////////////////////////////

} // namespace gui
