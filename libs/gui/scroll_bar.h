// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "widget.h"

#include <base/signal.h>
#include <draw/rectangle.h>

namespace gui
{
////////////////////////////////////////

class scroll_bar_w : public widget
{
public:
    using value_type = coord::value_type;

    scroll_bar_w( bool bounded = true );
    ~scroll_bar_w( void ) override;

    value_type value( void ) const { return _value; }

    void set_horizontal( void ) { _horizontal = true; }

    void set_vertical( void ) { _horizontal = false; }

    void set_value( value_type v );
    void set_handle( value_type h );
    void set_page( value_type p );
    void set_range( value_type min, value_type max );

    void build( context &ctxt ) override;
    void paint( context &ctxt ) override;

    bool mouse_press( const event &e ) override;
    bool mouse_move( const event &e ) override;
    bool mouse_release( const event &e ) override;

    signal<void( value_type )> when_changing;

private:
    void update_value( value_type v );

    value_type translate_to_full_w( value_type v )
    {
        return _min + ( v - x1().count() ) * ( _max - _min ) / width().count();
    }

    value_type translate_from_full_w( value_type v )
    {
        return x1().count() + ( v - _min ) * width().count() / ( _max - _min );
    }

    value_type translate_to_full_h( value_type v )
    {
        return _min + ( v - y1().count() ) * ( _max - _min ) / height().count();
    }

    value_type translate_from_full_h( value_type v )
    {
        return y1().count() + ( v - _min ) * height().count() / ( _max - _min );
    }

    value_type _start = value_type( 0 );

    value_type _value  = value_type( 0 );
    value_type _handle = value_type( 20 );
    value_type _page   = value_type( 0 );
    value_type _min = value_type( 0 ), _max = value_type( 100 );

    bool _tracking   = false;
    bool _horizontal = true;
    bool _bounded    = true;

    draw::rectangle _groove;
    draw::rectangle _knob;
};

////////////////////////////////////////

using scroll_bar = widget_ptr<scroll_bar_w>;

} // namespace gui
