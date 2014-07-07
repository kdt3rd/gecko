
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

slider::slider( double v, double min, double max )
	: _value( v ), _min( min ), _max( max )
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
	if ( _min != min || _max != max )
	{
		_min = min;
		_max = max;
		when_range_changed( _min, _max );
	}
}

////////////////////////////////////////

void slider::set_value( double v )
{
	v = std::max( _min, std::min( _max, v ) );
	if ( v != _value )
	{
		_value = v;
		invalidate();
	}
}

////////////////////////////////////////

void slider::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	if ( !_groove )
	{
		base::path path;
		path.rounded_rect( { 0, 0 }, { 20, 7 }, 2 );

		base::paint paint;
		paint.set_fill_color( { 0.27, 0.27, 0.27 } );

		_groove = std::make_shared<draw::stretchable>();
		_groove->create( canvas, path, paint, { 10, 3.5 } );
	}

	if ( !_knob )
	{
		base::path path;
		path.circle( { 10, 10 }, 9 );

		base::paint paint;
		paint.set_fill_color( { 0.57, 0.57, 0.57 } );

		_knob = std::make_shared<draw::stretchable>();
		_knob->create( canvas, path, paint, { 10, 10 } );
	}

	base::rect r = *this;

	{
		double rad = this->radius();
		double h = this->height() - 7;
		base::rect tmp( *this );
		tmp.trim( rad, rad, h/2, h/2 );
		_groove->set( canvas, tmp );
		_groove->draw( *canvas );
	}

	{
		double rad = 9.0;
		base::rect tmp( rad * 2, rad * 2 );
		tmp.set_center( { r.x( _value, rad ), r.y( 0.5, rad ) } );

		_knob->set( canvas, tmp );
		_knob->draw( *canvas );
	}
}

////////////////////////////////////////

void slider::compute_minimum( void )
{
	set_minimum( 17.0 * 2.0, 17.0 );
}

////////////////////////////////////////

bool slider::mouse_press( const base::point &p, int button )
{
	if ( contains( p ) )
	{
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
		if ( z1 < z2 )
		{
			double current = z1 + _value * ( z2 - z1 );
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

bool slider::mouse_move( const base::point &p )
{
	if ( _tracking )
	{
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
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

bool slider::mouse_release( const base::point &p, int button )
{
	if ( _tracking )
	{
		_pressed = false;
		_tracking = false;
		double z1 = x1() + _handle;
		double z2 = x2() - _handle;
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

