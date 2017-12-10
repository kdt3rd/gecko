//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "slider.h"
#include "application.h"


namespace gui
{

////////////////////////////////////////

slider::slider( void )
{
}

////////////////////////////////////////

slider::slider( slider_value v, slider_value min, slider_value max )
	: _value( v ), _min( min ), _max( max )
{
}

////////////////////////////////////////

slider::~slider( void )
{
}

////////////////////////////////////////

void slider::set_range( slider_value min, slider_value max )
{
	precondition( min < max, "invalid range" );
	if ( ! std::equal_to<slider_value>()(_min, min) || ! std::equal_to<slider_value>()(_max, max) )
	{
		_min = min;
		_max = max;
		when_range_changed( _min, _max );
	}
}

////////////////////////////////////////

void slider::set_value( slider_value v )
{
	v = std::max( _min, std::min( _max, v ) );
	if ( ! std::equal_to<slider_value>()(v, _value) )
	{
		_value = v;
		invalidate();
	}
}

////////////////////////////////////////

void slider::build( gl::api &ogl )
{
	const style &s = context::current().get_style();
	_groove.set_color( s.secondary_text( s.background_color() ) );

	draw::path handle;
	handle.circle( { 0, 0 }, _handle );

	draw::paint paint;
	paint.set_fill_color( s.dominant_color() );

	_knob.add( ogl, handle, paint );

	layout_target()->set_minimum( 24.0 * 2.0, 24.0 );
}

////////////////////////////////////////

void slider::paint( gl::api &ogl )
{
	coord_type ypos = y() + height()/2.0;

	_groove.set_position( x(), ypos - 1 );
	_groove.set_size( width(), 2 );
	_groove.draw( ogl );

	_knob.set_position( x( _value, _handle ), ypos );
	_knob.draw( ogl );
}

////////////////////////////////////////

bool slider::mouse_press( const point &p, int /*button*/ )
{
	if ( contains( p ) )
	{
		slider_value z1 = x1() + _handle;
		slider_value z2 = x2() - _handle;
		if ( z1 < z2 )
		{
			slider_value current = z1 + _value * ( z2 - z1 );
			slider_value dist = std::abs( p.x() - current );
			if ( dist < _handle )
			{
				_tracking = true;
				_start = p.x();
				invalidate(); // TODO invalidate only handle.
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_move( const point &p )
{
	if ( _tracking )
	{
		slider_value z1 = x1() + _handle;
		slider_value z2 = x2() - _handle;
		if ( z1 < z2 )
			set_value( ( p.x() - z1 ) / ( z2 - z1 ) );
		else
			set_value( ( _min + _max ) / 2.0 );
		when_changing( _value );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_release( const point &p, int /*button*/ )
{
	if ( _tracking )
	{
		_tracking = false;
		slider_value z1 = x1() + _handle;
		slider_value z2 = x2() - _handle;
		if ( z1 < z2 )
			set_value( ( p.x() - z1 ) / ( z2 - z1 ) );
		else
			set_value( ( _min + _max ) / 2.0 );
		when_changed( _value );
		invalidate();
		return true;
	}
	return false;
}

////////////////////////////////////////

}

