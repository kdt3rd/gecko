// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "scroll_bar.h"

#include <base/contract.h>

namespace gui
{
////////////////////////////////////////

scroll_bar_w::scroll_bar_w( bool bounded ) : _bounded( bounded ) {}

////////////////////////////////////////

scroll_bar_w::~scroll_bar_w( void ) {}

////////////////////////////////////////

void scroll_bar_w::set_value( value_type v )
{
    _value = v;
    update_value( _value );
}

////////////////////////////////////////

void scroll_bar_w::set_handle( value_type h )
{
    precondition(
        h >= value_type( 0 ), "invalid scroll bar handle size {0}", h );
    _handle = h;
    if ( _handle > _max - _min )
        _handle = _max - _min;
    update_value( _value );
}

////////////////////////////////////////

void scroll_bar_w::set_page( value_type p ) { _page = p; }

////////////////////////////////////////

void scroll_bar_w::set_range( value_type min, value_type max )
{
    precondition( min < max, "invalid range" );
    if ( min != _min || max != _max )
    {
        _min = min;
        _max = max;
        update_value( _value );
    }
}

////////////////////////////////////////

void scroll_bar_w::build( context &ctxt )
{
    const style &s = ctxt.get_style();
    _groove.set_color( s.disabled_text( s.background_color() ) );
    _knob.set_color( s.dominant_color() );

    size sz = s.widget_minimum_size();
    if ( _horizontal )
        layout_target()->set_minimum( sz.w() * 4.F, sz.h() );
    else
        layout_target()->set_minimum( sz.w(), sz.h() * 4.F );
}

////////////////////////////////////////

void scroll_bar_w::paint( context &ctxt )
{
    platform::context &hwc = ctxt.hw_context();

    _groove.set_position( x(), y() );
    _groove.set_size( width(), height() );
    _groove.draw( hwc );

    if ( _horizontal )
    {
        value_type x = translate_from_full_w( _value );
        value_type w = _handle * width().count() / ( _max - _min );
        _knob.set_position( x, y() );
        _knob.set_size( w, height() );
        _knob.draw( hwc );
    }
    else
    {
        value_type y = translate_from_full_h( _value );
        value_type h = _handle * height().count() / ( _max - _min );
        _knob.set_position( x(), y );
        _knob.set_size( width(), h );
        _knob.draw( hwc );
    }
}

////////////////////////////////////////

bool scroll_bar_w::mouse_press( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    point p = e.from_native( e.raw_mouse().x, e.raw_mouse().y );

    if ( _horizontal )
    {
        value_type x1 = translate_from_full_w( _value );
        value_type x2 = translate_from_full_w( _value + _handle );
        if ( p[0].count() >= x1 && p[0].count() <= x2 )
        {
            _tracking = true;
            _start    = p[0].count();
            invalidate();
        }
        else if ( p[0].count() < x1 )
        {
            if ( _page > value_type( 0 ) )
                update_value( _value - _page );
            else
                update_value( _value - _handle / value_type( 2 ) );
        }
        else if ( p[0].count() > x2 )
        {
            if ( _page > value_type( 0 ) )
                update_value( _value + _page );
            else
                update_value( _value + _handle / value_type( 2 ) );
        }
    }
    else
    {
        value_type y1 = translate_from_full_h( _value );
        value_type y2 = translate_from_full_h( _value + _handle );
        if ( p[1].count() >= y1 && p[1].count() <= y2 )
        {
            _tracking = true;
            _start    = p[1].count();
            invalidate();
        }
        else if ( p[1].count() < y1 )
        {
            if ( _page > value_type( 0 ) )
                update_value( _value - _page );
            else
                update_value( _value - _handle / value_type( 2 ) );
        }
        else if ( p[1].count() > y2 )
        {
            if ( _page > value_type( 0 ) )
                update_value( _value + _page );
            else
                update_value( _value + _handle / value_type( 2 ) );
        }
    }

    if ( _tracking )
        context::current().grab_source( e, shared_from_this() );

    return _tracking;
}

////////////////////////////////////////

bool scroll_bar_w::mouse_move( const event &e )
{
    if ( _tracking )
    {
        point p = e.from_native( e.raw_mouse().x, e.raw_mouse().y );

        if ( _horizontal )
        {
            value_type delta = p[0].count() - _start;
            _start           = p[0].count();
            value_type x     = translate_from_full_w( _value );
            x += delta;
            update_value( translate_to_full_w( x ) );
        }
        else
        {
            value_type delta = p[1].count() - _start;
            _start           = p[1].count();
            value_type y     = translate_from_full_h( _value );
            y += delta;
            update_value( translate_to_full_h( y ) );
        }
        return true;
    }
    return false;
}

////////////////////////////////////////

bool scroll_bar_w::mouse_release( const event &e )
{
    if ( _tracking )
    {
        context::current().release_source( e );
        _tracking = false;
        return true;
    }
    return false;
}

////////////////////////////////////////

void scroll_bar_w::update_value( value_type v )
{
    bool changed = ( v != _value );
    _value       = v;
    if ( _value < _min )
    {
        if ( _bounded )
            _value = _min;
        else
        {
            if ( _page > value_type( 0 ) )
                _min = _value - _page;
            else
                _min = _value - _handle / value_type( 2 );
        }
        changed = true;
    }
    if ( _value > _max - _handle )
    {
        if ( _bounded )
            _value = _max - _handle;
        else
        {
            if ( _page > value_type( 0 ) )
                _max = _value + _handle + _page;
            else
                _max = _value + _handle + _handle / value_type( 2 );
        }
        changed = true;
    }
    if ( changed )
    {
        when_changing( _value );
        invalidate();
    }
}

////////////////////////////////////////

} // namespace gui
