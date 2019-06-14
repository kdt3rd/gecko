// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include "slider.h"

#include "application.h"

#include <iostream>

namespace gui
{
////////////////////////////////////////

slider_w::slider_w( void ) {}

////////////////////////////////////////

slider_w::slider_w( value_type v, value_type min, value_type max )
    : _value( v ), _min( min ), _max( max )
{}

////////////////////////////////////////

slider_w::~slider_w( void ) {}

////////////////////////////////////////

void slider_w::set_range( value_type min, value_type max )
{
    precondition( min < max, "invalid range" );
    if ( !std::equal_to<value_type>()( _min, min ) ||
         !std::equal_to<value_type>()( _max, max ) )
    {
        _min = min;
        _max = max;
        when_range_changed( _min, _max );
    }
}

////////////////////////////////////////

void slider_w::set_value( value_type v )
{
    v = std::max( _min, std::min( _max, v ) );
    if ( !std::equal_to<value_type>()( v, _value ) )
    {
        _value = v;
        invalidate();
    }
}

////////////////////////////////////////

void slider_w::build( context &ctxt )
{
    const style &s = ctxt.get_style();
    _groove.set_color( s.secondary_text( s.background_color() ) );

    gl::api &ogl = ctxt.hw_context().api();

    draw::path handle;
    handle.circle( { 0, 0 }, _handle );

    draw::paint paint;
    paint.set_fill_color( s.dominant_color() );

    _knob.add( ogl, handle, paint );

    size sz = s.widget_minimum_size();
    _knob.shape_size( _handle * 2.F, _handle * 2.F );
    _knob.set_size( sz.w(), sz.h() );

    layout_target()->set_minimum( sz.w() * 2.f, sz.h() );
}

////////////////////////////////////////

void slider_w::paint( context &ctxt )
{
    coord_type ypos = y() + height() / coord_type( 2 );

    coord_type         grvht = ctxt.from_native_vert( 2 );
    platform::context &c     = ctxt.hw_context();
    _groove.set_position( x(), ypos - grvht / coord_type( 2 ) );
    _groove.set_size( width(), grvht );
    _groove.draw( c );

    _knob.set_position( x( _value, _handle ), ypos );
    _knob.draw( c );
}

////////////////////////////////////////

bool slider_w::mouse_press( const event &e )
{
    if ( e.raw_mouse().button != 1 )
        return false;

    value_type z1 = x1() + _handle;
    value_type z2 = x2() - _handle;
    if ( z1 < z2 )
    {
        value_type current = z1 + _value * ( z2 - z1 );
        value_type ex      = e.from_native_horiz( e.raw_mouse().x );
        value_type dist    = std::abs( ex - current );
        if ( dist < _handle )
        {
            _tracking = true;
            context::current().grab_source( e, shared_from_this() );
            _start = ex;
            invalidate(); // TODO invalidate only handle.
            return true;
        }
    }

    return false;
}

////////////////////////////////////////

bool slider_w::mouse_move( const event &e )
{
    if ( _tracking )
    {
        value_type z1 = x1() + _handle;
        value_type z2 = x2() - _handle;
        value_type ex = e.from_native_horiz( e.raw_mouse().x );
        if ( z1 < z2 )
            set_value( ( ex - z1 ) / ( z2 - z1 ) );
        else
            set_value( ( _min + _max ) / value_type( 2 ) );
        when_changing( _value );
        return true;
    }
    return false;
}

////////////////////////////////////////

bool slider_w::mouse_release( const event &e )
{
    if ( _tracking )
    {
        _tracking     = false;
        value_type z1 = x1() + _handle;
        value_type z2 = x2() - _handle;
        value_type ex = e.from_native_horiz( e.raw_mouse().x );
        if ( z1 < z2 )
            set_value( ( ex - z1 ) / ( z2 - z1 ) );
        else
            set_value( ( _min + _max ) / value_type( 2 ) );
        when_changed( _value );
        invalidate();
        context::current().release_source( e );
        return true;
    }
    return false;
}

////////////////////////////////////////

} // namespace gui
