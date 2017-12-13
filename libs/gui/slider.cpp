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

slider_w::slider_w( void )
{
}

////////////////////////////////////////

slider_w::slider_w( value_type v, value_type min, value_type max )
	: _value( v ), _min( min ), _max( max )
{
}

////////////////////////////////////////

slider_w::~slider_w( void )
{
}

////////////////////////////////////////

void slider_w::set_range( value_type min, value_type max )
{
	precondition( min < max, "invalid range" );
	if ( ! std::equal_to<value_type>()(_min, min) || ! std::equal_to<value_type>()(_max, max) )
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
	if ( ! std::equal_to<value_type>()(v, _value) )
	{
		_value = v;
		invalidate();
	}
}

////////////////////////////////////////

void slider_w::build( gl::api &ogl )
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

void slider_w::paint( gl::api &ogl )
{
	coord_type ypos = y() + height()/2.0;

	_groove.set_position( x(), ypos - 1 );
	_groove.set_size( width(), 2 );
	_groove.draw( ogl );

	_knob.set_position( x( _value, _handle ), ypos );
	_knob.draw( ogl );
}

////////////////////////////////////////

bool slider_w::mouse_press( const point &p, int /*button*/ )
{
	if ( contains( p ) )
	{
		value_type z1 = x1() + _handle;
		value_type z2 = x2() - _handle;
		if ( z1 < z2 )
		{
			value_type current = z1 + _value * ( z2 - z1 );
			value_type dist = std::abs( p.x() - current );
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

bool slider_w::mouse_move( const point &p )
{
	if ( _tracking )
	{
		value_type z1 = x1() + _handle;
		value_type z2 = x2() - _handle;
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

bool slider_w::mouse_release( const point &p, int /*button*/ )
{
	if ( _tracking )
	{
		_tracking = false;
		value_type z1 = x1() + _handle;
		value_type z2 = x2() - _handle;
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

