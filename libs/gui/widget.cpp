// Copyright (c) 2015 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "widget.h"

#include <base/animation.h>
#include <base/contract.h>

////////////////////////////////////////

namespace gui
{
////////////////////////////////////////

widget::widget( void )
    : rect( 0, 0, 0, 0 ), _area{ std::make_shared<layout::area>( 0, 0, 0, 0 ) }
{}

////////////////////////////////////////

widget::widget( std::unique_ptr<layout::area> &&a )
    : rect( 0, 0, 0, 0 ), _area( std::move( a ) )
{}

////////////////////////////////////////

widget::~widget( void ) {}

////////////////////////////////////////

void widget::monitor_changed( context & ) {}

////////////////////////////////////////

void widget::build( context & ) {}

////////////////////////////////////////

void widget::paint( context & ) {}

////////////////////////////////////////

std::shared_ptr<widget> widget::find_widget_under( const point &p )
{
    if ( contains( p ) )
        return shared_from_this();

    return std::shared_ptr<widget>();
}

////////////////////////////////////////

bool widget::mouse_press( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::mouse_release( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::mouse_move( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::mouse_wheel( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::key_press( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::key_repeat( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::key_release( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::text_input( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::tablet_event( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::hid_event( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::user_event( const event &e )
{
    unused( e );
    return false;
}

////////////////////////////////////////

bool widget::update_layout( double duration )
{
    precondition( _area, "null area" );
    bool result = false;
    if ( distance( *this, *_area ) > coord( 1 ) )
    {
        if ( _anim_time < 0.0 )
        {
            _anim_start = *this;
            _anim_time  = 0.0;
        }
        _anim_time += 16.66666;

        if ( _anim_time > duration )
        {
            set_position( _area->position() );
            set_extent( _area->extent() );
            _anim_time = -1.0;
        }
        else
        {
            point tl = base::ease(
                _anim_start.top_left(),
                _area->top_left(),
                _anim_time,
                duration );
            point br = base::ease(
                _anim_start.bottom_right(),
                _area->bottom_right(),
                _anim_time,
                duration );
            set_x1( tl[0] );
            set_y1( tl[1] );
            set_x2( br[0] );
            set_y2( br[1] );
            result = true;
        }
    }
    else
    {
        set_position( _area->position() );
        set_extent( _area->extent() );
        _anim_time = -1.0;
    }
    return result;
}

////////////////////////////////////////

} // namespace gui
