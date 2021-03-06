// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "radio_button.h"

#include "application.h"

#include <draw/icons.h>
#include <draw/paint.h>
#include <draw/path.h>
#include <iostream>

namespace gui
{
////////////////////////////////////////

radio_button_w::radio_button_w( void ) {}

////////////////////////////////////////

radio_button_w::~radio_button_w( void ) {}

////////////////////////////////////////

void radio_button_w::build( context &ctxt )
{
    const style &s   = ctxt.get_style();
    gl::api &    ogl = ctxt.hw_context().api();

    draw::paint c;
    c.set_fill_color( s.active_icon( s.background_color() ) );
    _unchecked.add( ogl, draw::iconRadioButtonEmpty(), c );

    c.set_fill_color( s.dominant_color() );
    _checked.add( ogl, draw::iconRadioButtonChecked(), c );

    size sz = s.widget_minimum_size();

    _unchecked.shape_size( 24, 24 );
    _checked.shape_size( 24, 24 );
    _unchecked.set_size( sz.w(), sz.h() );
    _checked.set_size( sz.w(), sz.h() );

    auto &l = layout_target();
    l->set_minimum( sz );
    l->set_maximum( sz );
}

////////////////////////////////////////

void radio_button_w::paint( context &ctxt )
{
    auto sz   = std::min( width(), height() );
    auto offx = x() + ( width() - sz ) / 2.f;
    auto offy = y() + ( height() - sz ) / 2.f;
    _checked.set_position( offx, offy );
    _unchecked.set_position( offx, offy );

    bool c = _state;
    if ( _tracking )
        c = _current;
    if ( c )
    {
        _checked.set_size( sz, sz );
        _checked.draw( ctxt.hw_context() );
    }
    else
    {
        _unchecked.set_size( sz, sz );
        _unchecked.draw( ctxt.hw_context() );
    }
}

////////////////////////////////////////

bool radio_button_w::mouse_press( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    context::current().grab_source( e, shared_from_this() );

    _tracking = true;
    _current  = true;
    invalidate();

    return true;
}

////////////////////////////////////////

bool radio_button_w::mouse_release( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    if ( _tracking )
    {
        _tracking = false;
        if ( contains( e.from_native( e.raw_mouse().x, e.raw_mouse().y ) ) )
            set_state( _current );
        invalidate();
        context::current().release_source( e );
        return true;
    }
    return false;
}

////////////////////////////////////////

bool radio_button_w::mouse_move( const event &e )
{
    if ( _tracking )
    {
        if ( contains( e.from_native( e.raw_mouse().x, e.raw_mouse().y ) ) )
            _current = true;
        else
            _current = _state;
        invalidate();
        return true;
    }
    return false;
}

////////////////////////////////////////

void radio_button_w::set_state( bool s )
{
    if ( _state != s )
    {
        _state = s;
        if ( _state )
            when_selected();
    }
}

////////////////////////////////////////

} // namespace gui
