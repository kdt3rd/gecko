//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "widget.h"
#include <base/animation.h>
#include <base/contract.h>

////////////////////////////////////////

namespace gui
{

////////////////////////////////////////

widget::widget( void )
	: _area{ std::make_shared<layout::area>() }
{
}

////////////////////////////////////////

widget::widget( std::unique_ptr<layout::area> &&a )
	: _area( std::move( a ) )
{
}

////////////////////////////////////////

widget::~widget( void )
{
}

////////////////////////////////////////

void widget::monitor_changed( context & )
{
}

////////////////////////////////////////

void widget::build( context & )
{
}

////////////////////////////////////////

void widget::paint( context & )
{
}

////////////////////////////////////////

bool widget::mouse_press( const point &p, int button )
{
	unused( p );
	unused( button );
	return false;
}

////////////////////////////////////////

bool widget::mouse_release( const point &p, int button )
{
	unused( p );
   	unused( button );
	return false;
}

////////////////////////////////////////

bool widget::mouse_move( const point &p )
{
	unused( p );
	return false;
}

////////////////////////////////////////

bool widget::mouse_wheel( int a )
{
	unused( a );
	return false;
}

////////////////////////////////////////

bool widget::key_press( platform::scancode c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

bool widget::key_release( platform::scancode c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

bool widget::text_input( char32_t c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

bool widget::update_layout( double duration )
{
	precondition( _area, "null area" );
	bool result = false;
	if ( distance( *_area ) > 0.001 )
	{
		if ( _anim_time < 0.0 )
		{
			_anim_start = *this;
			_anim_time = 0.0;
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
			point tl = base::ease( _anim_start.top_left(), _area->top_left(), _anim_time, duration );
			point br = base::ease( _anim_start.bottom_right(), _area->bottom_right(), _anim_time, duration );
			set_x1( tl.x() );
			set_y1( tl.y() );
			set_x2( br.x() );
			set_y2( br.y() );
			result = true;
		}
	}
	else
		_anim_time = -1.0;
	return result;
}
////////////////////////////////////////

}



