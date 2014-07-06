
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

color_picker::color_picker( base::color::space space )
	: _space( space ), _current( { 1, 0, 0, 1 } )
{
	set_minimum( 120, 120 );
}

////////////////////////////////////////

namespace
{
	/*
	inline double radial_square( double a )
	{
		a = std::fmod( a + PI/4, PI/2 ) - PI/4;
		return 1 / std::cos( a );
	}
	*/
}

void color_picker::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	base::path path;
	base::paint paint;
	draw::object obj;

	/*
	base::rect tmp( radius() * 2, radius() * 2 );
	tmp.set_center( center() );
	path.rounded_rect( tmp, 10.0 );
	path.circle( center(), radius()/2.0 );
	paint.set_fill_color( { 0.84, 0.84, 0.84 } );
	obj.create( canvas, path, paint );
	obj.draw( *canvas );
	*/

	path.clear();
	path.circle( center(), radius()*0.95 );
	path.circle( center(), radius()*0.7 );

	base::gradient grad;
	for ( size_t i = 0; i <= 36; ++i )
	{
		double v = i / 36.0;
		base::color c( base::color::space::HSL, v * 2.0 * PI, 0.75, 0.5 );
		grad.add_stop( v, c );
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
		double r1 = radius();
		double r2 = std::max( r1 - 24, 0.0 );
		if ( base::point::distance( p, center() ) > r2 )
		{
			_tracking = true;
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
		double h, s, l;
		_current.get_hsl( h, s, l );
		h = std::atan2( d.y(), d.x() );
		_current.set_hsl( h, s, l );
		invalidate();
		return true;
	}

	return widget::mouse_move( p );
}

////////////////////////////////////////

}

