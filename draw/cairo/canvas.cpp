
#include "canvas.h"
#include "font.h"
#include <draw/path.h>
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
	if( _context )
	{
		cairo_surface_finish( _surface );
		cairo_destroy( _context );
		_context = nullptr;
		_surface = nullptr;
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

void canvas::fill( const draw::rect &r, const draw::paint &c )
{
	cairo_rectangle( _context, r.x(), r.y(), r.width(), r.height() );
	if( !_context )
		return;

	set_cairo( c );
	if ( !set_cairo_fill( c ) )
		set_cairo_stroke( c );
	cairo_fill( _context );
	check_error();
}

////////////////////////////////////////

void canvas::draw_path( const draw::path &path, const draw::paint &c )
{
	cairo_save( _context );
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
				if ( a.x() < a.y() )
					cairo_arc( _context, c.x(), c.y(), draw::point::distance( c, r ), a.x(), a.y() );
				else
					cairo_arc_negative( _context, c.x(), c.y(), draw::point::distance( c, r ), a.x(), a.y() );
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
	cairo_restore( _context );
}

////////////////////////////////////////

void canvas::draw_text( const std::shared_ptr<draw::font> &font, const draw::point &p, const std::string &utf8, const draw::paint &c )
{
	cairo_save( _context );

	set_cairo_font( font );

	cairo_move_to( _context, p.x(), p.y() );
	cairo_text_path( _context, utf8.c_str() );

	set_cairo( c );
	if ( set_cairo_fill( c ) )
		cairo_fill_preserve( _context );
	set_cairo_stroke( c );
	cairo_stroke( _context );

	check_error();
	cairo_restore( _context );
}

////////////////////////////////////////

void canvas::clip( const draw::rect &r )
{
	cairo_rectangle( _context, r.x(), r.y(), r.width(), r.height() );
	cairo_clip( _context );
}

////////////////////////////////////////

void canvas::translate( double x, double y )
{
	cairo_translate( _context, x, y );
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

void canvas::save( void )
{
	cairo_save( _context );
}

////////////////////////////////////////

void canvas::restore( void )
{
	cairo_restore( _context );
}

////////////////////////////////////////

void canvas::screenshot_png( const char *filename )
{
	if ( !_surface )
		throw std::runtime_error( "missing surface" );

	cairo_surface_write_to_png( _surface, filename );
}

////////////////////////////////////////

void canvas::set_cairo( const draw::paint &p )
{
	cairo_set_antialias( _context, p.has_antialias() ? CAIRO_ANTIALIAS_SUBPIXEL : CAIRO_ANTIALIAS_NONE );
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

void canvas::set_cairo_font( const std::shared_ptr<draw::font> &bfont )
{
	auto *font = dynamic_cast<cairo::font*>( bfont.get() );
	precondition( font, "draw_text with null font" );

	cairo_set_scaled_font( _context, font->cairo_font() );
}

////////////////////////////////////////

void canvas::check_error( void )
{
	if ( _context )
		if ( cairo_status( _context ) )
			throw std::runtime_error( cairo_status_to_string( cairo_status( _context ) ) );
}

////////////////////////////////////////

}
