
#include "color_picker.h"
#include <draw/object.h>

namespace
{
	constexpr double PI = 3.14159265358979323846;
}

////////////////////////////////////////

namespace gui
{

////////////////////////////////////////

color_picker::color_picker( void )
	: _current( { 1, 0, 0, 1 } )
{
	set_minimum( 120, 120 );
}

////////////////////////////////////////

namespace
{
}

void color_picker::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	base::path path;
	base::paint paint;
	base::gradient grad;
	draw::object obj;

	// Draw the background and sample
	grad.add_stop( 0.0, _current );
	grad.add_stop( 0.6666, _current );
	grad.add_stop( 0.6666, base::color::white );
	grad.add_stop( 1.0, base::color::white );

	path.circle( center(), radius()*0.70 );
	paint.set_fill_radial( center(), radius()*0.70, grad );

	obj.create( canvas, path, paint );
	obj.draw( *canvas );

	path.clear();
	path.circle( center(), radius()*0.95 );
	path.circle( center(), radius()*0.70 );

	grad.clear();
	base::color c;
	for ( size_t i = 0; i <= 36; ++i )
	{
		double v = i / 36.0;
		c.set_hsl( v * 2.0 * PI, 1.0, 0.5 );
		grad.add_stop( v, base::color( base::color::space::HSL, v * 2.0 * PI, 1.0, 0.5 ) );
	}
	paint.set_fill_conical( center(), grad );

	obj.create( canvas, path, paint );
	obj.draw( *canvas );
}

////////////////////////////////////////

bool color_picker::mouse_press( const base::point &p, int b )
{
	if ( b == 1 )
	{
		double r = base::point::distance( p, center() ) / radius();
		if ( r > 0.70 && r < 0.95 )
		{
			_tracking = true;
			base::point d = p.delta( center() );
			double h = std::atan2( -d.x(), -d.y() );
			_current.set_hsl( h, 1.0, 0.5 );
			return mouse_move( p );
		}
	}

	return widget::mouse_press( p, b );
}

////////////////////////////////////////

bool color_picker::mouse_release( const base::point &p, int b )
{
	if ( _tracking && b == 1 )
	{
		_tracking = false;
		return true;
	}
	return widget::mouse_release( p, b );
}

////////////////////////////////////////

bool color_picker::mouse_move( const base::point &p )
{
	if ( _tracking )
	{
		base::point d = p.delta( center() );
		double h = std::atan2( -d.x(), -d.y() );
		_current.set_hsl( h, 1.0, 0.5 );
		invalidate();
		return true;
	}

	return widget::mouse_move( p );
}

////////////////////////////////////////

}

