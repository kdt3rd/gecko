
#include <iostream>
#include "slider.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

slider::slider( void )
{
	callback_invalidate( _value, _min, _max );
}

////////////////////////////////////////

slider::slider( datum<double> &&v, datum<double> &&min, datum<double> &&max )
	: _value( std::move( v ) ), _min( std::move( min ) ), _max( std::move( max ) )
{
	callback_invalidate( _value, _min, _max );
}

////////////////////////////////////////

slider::~slider( void )
{
}

////////////////////////////////////////

void slider::set_range( double min, double max )
{
	precondition( min < max, "invalid range" );
	_min = min;
	_max = max;
}

////////////////////////////////////////

void slider::set_value( double v )
{
	v = std::max( _min.value(), std::min( _max.value(), v ) );
	if ( v != _value.value() )
	{
		_value = v;
		invalidate();
	}
}

////////////////////////////////////////

void slider::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::rect r = *this;
	auto style = application::current()->get_style();
	style->slider_groove( canvas, r );
	style->slider_button( canvas, r, _pressed, _value.value() );
}

////////////////////////////////////////

void slider::compute_minimum( void )
{
	set_minimum( 17.0 * 2.0, 17.0 );
}

////////////////////////////////////////

bool slider::mouse_press( const draw::point &p, int button )
{
	if ( contains( p ) )
	{
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
		if ( z1 < z2 )
		{
			double current = z1 + _value.value() * ( z2 - z1 );
			double dist = std::abs( p.x() - current );
			if ( dist < _handle )
			{
				_tracking = true;
				_pressed = true;
				_start = p.x();
				invalidate(); // TODO invalidate only handle.
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_move( const draw::point &p )
{
	if ( _tracking )
	{
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
		if ( z1 < z2 )
			set_value( ( p.x() - z1 ) / ( z2 - z1 ) );
		else
			set_value( ( _min.value() + _max.value() ) / 2.0 );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_release( const draw::point &p, int button )
{
	if ( _tracking )
	{
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
		if ( z1 < z2 )
			set_value( ( p.x() - z1 ) / ( z2 - z1 ) );
		else
			set_value( ( _min.value() + _max.value() ) / 2.0 );
		_pressed = false;
		_tracking = false;
		invalidate();
		return true;
	}
	return false;
}

////////////////////////////////////////

}

