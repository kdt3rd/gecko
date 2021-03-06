// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "color_picker.h"

#include <draw/color_wheel.h>
#include <draw/object.h>

/*
namespace
{
	constexpr double PI = 3.14159265358979323846;
}
*/

////////////////////////////////////////

namespace gui
{
////////////////////////////////////////

color_picker::color_picker( void ) : _current( { 1, 0, 0, 1 } )
{
    set_minimum( 120, 120 );
}

////////////////////////////////////////

namespace
{}

void color_picker::paint( const std::shared_ptr<draw::canvas> &canvas )
{
    draw::color_wheel wheel;
    wheel.create( canvas, center(), radius() * 0.95 );
    wheel.draw( *canvas );

    // Draw the background and sample
    base::gradient grad;
    grad.add_stop( 0.0, _current );
    grad.add_stop( 0.6666, _current );
    grad.add_stop( 0.6666, base::white );
    grad.add_stop( 1.0, base::white );

    base::path path;
    path.circle( center(), radius() * 0.75 );

    base::paint paint;
    paint.set_fill_radial( center(), radius() * 0.75, grad );

    draw::object obj;
    obj.create( canvas, path, paint );
    obj.draw( *canvas );
}

////////////////////////////////////////

bool color_picker::mouse_press( const point &p, int b )
{
    if ( b == 1 )
    {
        coord_type r = point::distance( p, center() ) / radius();
        if ( r > 0.70 && r < 0.95 )
        {
            _tracking    = true;
            point      d = p.delta( center() );
            coord_type h = std::atan2( d.y(), d.x() );
            _current.set_hsl( h, 1.0, 0.5 );
            return mouse_move( p );
        }
    }

    return widget::mouse_press( p, b );
}

////////////////////////////////////////

bool color_picker::mouse_release( const point &p, int b )
{
    if ( _tracking && b == 1 )
    {
        _tracking = false;
        return true;
    }
    return widget::mouse_release( p, b );
}

////////////////////////////////////////

bool color_picker::mouse_move( const point &p )
{
    if ( _tracking )
    {
        point      d = p.delta( center() );
        coord_type h = std::atan2( d.y(), d.x() );
        _current.set_hsl( h, 1.0, 0.5 );
        invalidate();
        return true;
    }

    return widget::mouse_move( p );
}

////////////////////////////////////////

} // namespace gui
