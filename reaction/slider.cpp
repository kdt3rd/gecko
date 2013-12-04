
#include <iostream>
#include "slider.h"

namespace reaction
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

bool slider::mouse_press( const layout::simple_area &area, const draw::point &p, int button )
{
	if ( area.contains( p ) )
	{
		double x1 = area.x1() + _handle;
		double x2 = area.x2() - _handle;
		if ( x1 < x2 )
		{
			double current = x1 + _value * ( x2 - x1 );
			double dist = std::abs( p.x() - current );
			if ( dist < _handle )
			{
				_tracking = true;
				_start = p.x();
				move_began( _value );
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_move( const layout::simple_area &area, const draw::point &p )
{
	if ( _tracking )
	{
		double x1 = area.x1() + _handle;
		double x2 = area.x2() - _handle;
		if ( x1 < x2 )
		{
			double current = ( p.x() - x1 ) / ( x2 - x1 );
			changed( std::max( 0.0, std::min( 1.0, current ) ) );
		}
		else
			changed( 0.5 );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool slider::mouse_release( const layout::simple_area &area, const draw::point &p, int button )
{
	if ( _tracking )
	{
		_tracking = false;
		double x1 = area.x1() + _handle;
		double x2 = area.x2() - _handle;
		if ( x1 < x2 )
		{
			_value = ( p.x() - x1 ) / ( x2 - x1 );
			_value = std::max( 0.0, std::min( 1.0, _value ) );
		}
		else
			_value = 0.5;

		move_ended( _value );
		return true;
	}
	return false;
}

////////////////////////////////////////

}

