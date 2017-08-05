//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "scroll_bar.h"
#include <base/contract.h>

namespace gui
{

////////////////////////////////////////

scroll_bar::scroll_bar( void )
{
}

////////////////////////////////////////

scroll_bar::~scroll_bar( void )
{
}

////////////////////////////////////////

void scroll_bar::set_value( double v )
{
	_value = v;
	fix_value();
	when_changing( v );
	invalidate();
}

////////////////////////////////////////

void scroll_bar::set_handle( double h )
{
	_handle = h;
	fix_value();
	when_changing( _value );
	invalidate();
}

////////////////////////////////////////

void scroll_bar::set_range( double min, double max )
{
	precondition( min < max, "invalid range" );
	_min = min;
	_max = max;
	fix_value();
	when_changing( _value );
	invalidate();
}

////////////////////////////////////////

void scroll_bar::build( gl::api &ogl )
{
	const style &s = context::current().get_style();
	_groove.set_color( s.disabled_text( s.background_color() ) );
	_knob.set_color( s.dominant_color() );
}

////////////////////////////////////////

void scroll_bar::paint( gl::api &ogl )
{
	_groove.set_position( x(), y() );
	_groove.set_size( width(), height() );
	_groove.draw( ogl );

	if ( _horizontal )
	{
		double x = translate_from_full_w( _value );
		double w = _handle * width() / ( _max - _min );
		_knob.set_position( x, y() );
		_knob.set_size( w, height() );
		_knob.draw( ogl );
	}
	else
	{
		double y = translate_from_full_h( _value );
		double h = _handle * height() / ( _max - _min );
		_knob.set_position( x(), y );
		_knob.set_size( width(), h );
		_knob.draw( ogl );
	}
}

////////////////////////////////////////

void scroll_bar::compute_bounds( void )
{
	if ( _horizontal )
		set_minimum( 45.0, 15.0 );
	else
		set_minimum( 15.0, 45.0 );
}

////////////////////////////////////////

bool scroll_bar::mouse_press( const base::point &p, int button )
{
	if ( contains( p ) )
	{
		if ( _horizontal )
		{
			double x1 = translate_from_full_w( _value );
			double x2 = translate_from_full_w( _value + _handle );
			if ( p.x() >= x1 && p.x() <= x2 )
			{
				_tracking = true;
				_start = p.x();
				invalidate();
			}
		}
		else
		{
			double y1 = translate_from_full_h( _value );
			double y2 = translate_from_full_h( _value + _handle );
			if ( p.y() >= y1 && p.y() <= y2 )
			{
				_tracking = true;
				_start = p.y();
				invalidate();
			}
		}
	}
	return _tracking;
}

////////////////////////////////////////

bool scroll_bar::mouse_move( const base::point &p )
{
	if ( _tracking )
	{
		if ( _horizontal )
		{
			double delta = p.x() - _start;
			_start = p.x();
			double x = translate_from_full_w( _value );
			x += delta;
			_value = translate_to_full_w( x );
			fix_value();
			when_changing( _value );
			invalidate();
		}
		else
		{
			double delta = p.y() - _start;
			_start = p.y();
			double y = translate_from_full_h( _value );
			y += delta;
			_value = translate_to_full_h( y );
			fix_value();
			when_changing( _value );
			invalidate();
		}
		return true;
	}
	return false;
}

////////////////////////////////////////

bool scroll_bar::mouse_release( const base::point &p, int button )
{
	if ( _tracking )
	{
		_tracking = false;
		return true;
	}
	return false;
}

////////////////////////////////////////

void scroll_bar::fix_value( void )
{
	if ( _value < _min )
		_value = _min;
	if ( _value > _max - _handle )
		_value = _max - _handle;
}

////////////////////////////////////////

}

