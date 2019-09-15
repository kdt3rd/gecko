// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "widget.h"

#include <base/signal.h>
#include <draw/rectangle.h>
#include <draw/shape.h>

namespace gui
{
////////////////////////////////////////

class slider_w : public widget
{
public:
    using value_type = coord_type;

    slider_w( void );
    slider_w( value_type v, value_type min = 0.0, value_type max = 1.0 );
    ~slider_w( void ) override;

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    value( void ) const
    {
        value_type v = _value;
        v = std::max( v, value_type( std::numeric_limits<T>::min() ) );
        v = std::min( v, value_type( std::numeric_limits<T>::max() ) );
        return T( std::round( v ) );
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    value( void ) const
    {
        value_type v = _value;
        v = std::max( v, value_type( std::numeric_limits<T>::min() ) );
        v = std::min( v, value_type( std::numeric_limits<T>::max() ) );
        return v;
    }

    value_type value( void ) const { return _value; }

    void set_value( value_type v );
    void set_range( value_type min, value_type max );

    void build( context &ctxt ) override;
    void paint( context &ctxt ) override;

    bool mouse_press( const event &e ) override;
    bool mouse_move( const event &e ) override;
    bool mouse_release( const event &e ) override;

    signal<void( value_type )>             when_changing;
    signal<void( value_type )>             when_changed;
    signal<void( value_type, value_type )> when_range_changed;

private:
    draw::rectangle _groove;
    draw::shape     _knob;

    bool       _tracking = false;
    value_type _handle   = 8.0;
    value_type _start    = 0.0;
    value_type _value    = 0.5;
    value_type _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

using slider = widget_ptr<slider_w>;

} // namespace gui
