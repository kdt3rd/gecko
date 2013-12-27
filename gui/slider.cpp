
#include <iostream>
#include "slider.h"
#include "application.h"
#include "style.h"


namespace gui
{

////////////////////////////////////////

slider::slider( void )
{
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
	_value = v;
	invalidate();
}

////////////////////////////////////////

void slider::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	draw::rect r = *this;
	auto style = application::current()->get_style();
	style->slider_groove( canvas, r );
	style->slider_button( canvas, r, _pressed, _value );
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
		const double handle = radius();
		double z1 = x1() + handle;
		double z2 = x2() - handle;
		if ( z1 < z2 )
		{
			double current = z1 + _value * ( z2 - z1 );
			double dist = std::abs( p.x() - current );
			if ( dist < handle )
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
		const double handle = radius();
		double z1 = x1() + handle;
		double z2 = x2() - handle;
		if ( z1 < z2 )
		{
			double current = ( p.x() - z1 ) / ( z2 - z1 );
			_value = std::max( 0.0, std::min( 1.0, current ) );
		}
		else
			_value = 0.5;
		invalidate();
		return true;
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_release( const draw::point &p, int button )
{
	if ( _tracking )
	{
		const double handle = radius();
		double z1 = x1() + handle;
		double z2 = x2() - handle;
		if ( z1 < z2 )
		{
			_value = ( p.x() - z1 ) / ( z2 - z1 );
			_value = std::max( 0.0, std::min( 1.0, _value ) );
		}
		else
			_value = 0.5;

		_pressed = false;
		_tracking = false;
		invalidate();
		return true;
	}
	return false;
}

////////////////////////////////////////

}

