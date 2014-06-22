
#include "color_picker.h"

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
	using base::path;
	using base::point;
	using base::color;

	double r1 = radius();
	double r2 = std::max( r1 - 24, 0.0 );
	point c = center();

	const size_t n = 24;
	const size_t seg = n / 4;
	static_assert( n%4 == 0, "invalid number of segments" );
//	double slice = PI / n;

	base::paint paint;
	for ( size_t i = 0; i < n; ++i )
	{
		double a = i * 2 * PI / n;
		/* TODO
		double a1 = a - slice;
		double a2 = a + slice;
		path p( point::polar( r1, a1 ) + c );
		p.arc_to( c, r1, a1, a2 );
		p.line_to( point::polar( r2, a2 ) + c );
		p.arc_to( c, r2, a2, a1 );
		p.close();
		*/
		switch ( _space )
		{
			case color::space::SRGB:
			case color::space::HSL:
				paint.set_fill_color( { color::space::HSL, a, 0.8, 0.5 } );
				break;

			case color::space::LAB:
			{
				double x, y;
				switch ( i / seg )
				{
					case 0:
						x = double( i%seg ) / double(seg);
						y = 0.0;
						break;

					case 1:
						x = 1.0;
						y = double( i%seg ) / double(seg);
						break;

					case 2:
						x = ( seg - double( i%seg ) ) / double(seg);
						y = 1.0;
						break;

					case 3:
						x = 0.0;
						y = ( seg - double( i%seg ) ) / double(seg);
						break;

					default:
						throw std::runtime_error( "oops" );
				}


				x -= 0.5;
				y -= 0.5;
				paint.set_fill_color( { color::space::LAB, 0.5, x, y } );
			}
		}
//		canvas->draw_path( p, paint );

		switch ( _space )
		{
			case color::space::SRGB:
			case color::space::HSL:
			{
				double h, s, l;
				_current.get_hsl( h, s, l );

				base::path m;
				m.move_to( c + point::polar( r2, h ) );
				m.line_to( c + point::polar( r2, h + 2 * PI / 3 ) );
				m.line_to( c + point::polar( r2, h + 4 * PI / 3 ) );
				std::vector<color> c =
				{
					_current,
					color( color::space::HSL, h, 1.0, 1.0 ),
					color( color::space::HSL, h, 1.0, 0.0 ),
				};
//				paint.set_fill_mesh( m, c );
//				canvas->draw_path( m, paint );
				break;
			}

			case color::space::LAB:
				break;
		}
	}
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

