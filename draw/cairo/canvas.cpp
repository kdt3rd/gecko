
#include "canvas.h"
#include <draw/path.h>
#include <cairo-xcb.h>
#include <core/contract.h>

namespace cairo
{

////////////////////////////////////////

canvas::canvas( void )
{
}

////////////////////////////////////////

canvas::~canvas( void )
{
	if ( _context )
		cairo_destroy( _context );
}

////////////////////////////////////////

void canvas::set_surface( cairo_surface_t *surf )
{
	_surface = surf;
	precondition( _surface, "null surface" );

	_context = cairo_create( _surface );
	check_error();

	postcondition( _context, "context could not be created" );
}

////////////////////////////////////////

void canvas::clear_surface( void )
{
	cairo_destroy( _context );
	check_error();
	_context = nullptr;
	_surface = nullptr;
}

////////////////////////////////////////

void canvas::set_size( int w, int h )
{
	precondition( w > 0 && h > 0, "invalid surface size" );
	if ( _surface )
	{
		cairo_xcb_surface_set_size( _surface, w, h );
		cairo_surface_flush( _surface );
		check_error();
	}
}

////////////////////////////////////////

void canvas::fill( const draw::paint &c )
{
	if( !_context )
		return;

	set_cairo( c );
	if ( !set_cairo_fill( c ) )
		set_cairo_stroke( c );
	cairo_paint( _context );
	check_error();
}

////////////////////////////////////////

void canvas::draw_path( const draw::path &path, const draw::paint &c )
{
	if( !_context )
		return;

	size_t p = 0;
	for ( auto v: path.get_actions() )
	{
		switch ( v )
		{
			case draw::path::action::MOVE:
			{
				auto &p1 = path.get_point( p++ );
				cairo_move_to( _context, p1.x(), p1.y() );
				break;
			}

			case draw::path::action::LINE:
			{
				auto &p1 = path.get_point( p++ );
				cairo_line_to( _context, p1.x(), p1.y() );
				break;
			}

			case draw::path::action::QUADRATIC:
			{
				p += 2;
				throw std::runtime_error( "not yet implemented" );
//				break;
			}

			case draw::path::action::CUBIC:
			{
				auto &p1 = path.get_point( p++ );
				auto &p2 = path.get_point( p++ );
				auto &p3 = path.get_point( p++ );
				cairo_curve_to( _context, p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y() );
				break;
			}

			case draw::path::action::ARC:
			{
				auto &c = path.get_point( p++ );
				auto &r = path.get_point( p++ );
				auto &a = path.get_point( p++ );
				cairo_arc( _context, c.x(), c.y(), draw::point::distance( c, r ), a.x(), a.y() );
				break;
			}

			case draw::path::action::CLOSE:
			{
				cairo_close_path( _context );
				break;
			}
		}
	}
	postcondition ( p == path.get_points().size(), "too many points?" );

	set_cairo( c );
	if ( set_cairo_fill( c ) )
		cairo_fill_preserve( _context );
	set_cairo_stroke( c );
	cairo_stroke( _context );

	check_error();
}

////////////////////////////////////////

void canvas::present( void )
{
	if ( !_surface )
		return;

	cairo_surface_flush( _surface );
	check_error();
}

////////////////////////////////////////

void canvas::set_cairo( const draw::paint &p )
{
	cairo_set_antialias( _context, p.has_antialias() ? CAIRO_ANTIALIAS_GOOD : CAIRO_ANTIALIAS_NONE );
}

////////////////////////////////////////

void canvas::set_cairo_stroke( const draw::paint &p )
{
	const draw::color &c = p.get_stroke_color();
	cairo_set_source_rgba( _context, c.red(), c.green(), c.blue(), c.alpha() );
	cairo_set_line_width( _context, p.get_stroke_width() );
}

////////////////////////////////////////

bool canvas::set_cairo_fill( const draw::paint &p )
{
	if ( p.has_fill_color() )
	{
		const draw::color &c = p.get_fill_color();
		cairo_pattern_t *pat = cairo_pattern_create_rgba( c.red(), c.green(), c.blue(), c.alpha() );
		cairo_set_source( _context, pat );
		cairo_pattern_destroy( pat );
		return true;
	}
	else if ( p.has_fill_linear() )
	{
		const draw::point &p1 = p.get_fill_linear_p1();
		const draw::point &p2 = p.get_fill_linear_p2();
		cairo_pattern_t *pat = cairo_pattern_create_linear( p1.x(), p1.y(), p2.x(), p2.y() );
		for ( auto s: p.get_fill_linear_stops() )
		{
			const draw::color &c = s.second;
			cairo_pattern_add_color_stop_rgba( pat, s.first, c.red(), c.green(), c.blue(), c.alpha() );
		}
		cairo_set_source( _context, pat );
		cairo_pattern_destroy( pat );
		return true;
	}
	else if ( p.has_fill_radial() )
	{
		const draw::point &p1 = p.get_fill_radial_p1();
		const draw::point &p2 = p.get_fill_radial_p2();
		double r1 = p.get_fill_radial_r1();
		double r2 = p.get_fill_radial_r2();

		cairo_pattern_t *pat = cairo_pattern_create_radial( p1.x(), p1.y(), r1, p2.x(), p2.y(), r2 );
		for ( auto s: p.get_fill_radial_stops() )
		{
			const draw::color &c = s.second;
			cairo_pattern_add_color_stop_rgba( pat, s.first, c.red(), c.green(), c.blue(), c.alpha() );
		}
		cairo_set_source( _context, pat );
		cairo_pattern_destroy( pat );
		return true;
	}

	return false;
}

////////////////////////////////////////

void canvas::check_error( void )
{
	if ( cairo_status( _context ) )
		throw std::runtime_error( cairo_status_to_string( cairo_status( _context ) ) );
}

////////////////////////////////////////

}
