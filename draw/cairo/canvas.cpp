
#include "canvas.h"
#include <draw/path.h>
#include <cairo-xcb.h>
#include <core/contract.h>

namespace cairo
{

////////////////////////////////////////

canvas::canvas( cairo_surface_t *surf )
	: _surface( surf ), _context( cairo_create( _surface ) )
{
	precondition( _surface, "null surface" );
	postcondition( _context, "context could not be created" );
}

////////////////////////////////////////

canvas::~canvas( void )
{
	cairo_destroy( _context );
}

////////////////////////////////////////

void canvas::fill( const draw::paint &c )
{
	set_cairo_source( c );
	cairo_paint( _context );
}

////////////////////////////////////////

void canvas::draw_path( const draw::path &path, const draw::paint &c )
{
	set_cairo_source( c );
	size_t p = 0;
	for ( auto v: path.get_verbs() )
	{
		switch ( v )
		{
			case draw::path::verb::MOVE:
			{
				auto &p1 = path.get_point( p++ );
				cairo_move_to( _context, p1.x(), p1.y() );
				break;
			}

			case draw::path::verb::LINE:
			{
				auto &p1 = path.get_point( p++ );
				cairo_line_to( _context, p1.x(), p1.y() );
				break;
			}

			case draw::path::verb::QUADRATIC:
			{
				p += 2;
				throw std::runtime_error( "not yet implemented" );
				break;
			}

			case draw::path::verb::CUBIC:
			{
				auto &p1 = path.get_point( p++ );
				auto &p2 = path.get_point( p++ );
				auto &p3 = path.get_point( p++ );
				cairo_curve_to( _context, p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y() );
				break;
			}

			case draw::path::verb::ARC:
			{
				auto &c = path.get_point( p++ );
				auto &r = path.get_point( p++ );
				auto &a = path.get_point( p++ );
				cairo_arc( _context, c.x(), c.y(), draw::point::distance( c, r ), a.x(), a.y() );
				break;
			}

			case draw::path::verb::CLOSE:
			{
				cairo_close_path( _context );
				break;
			}
		}
	}

	cairo_stroke( _context );
}

////////////////////////////////////////

void canvas::present( void )
{
	cairo_surface_flush( _surface );
	if ( cairo_status( _context ) )
		throw std::runtime_error( cairo_status_to_string( cairo_status( _context ) ) );
}

////////////////////////////////////////

void canvas::set_size( int w, int h )
{
	cairo_xcb_surface_set_size( _surface, w, h );
}

////////////////////////////////////////

void canvas::set_cairo_source( const draw::paint &p )
{
	const draw::color &c = p.get_color();
	cairo_set_source_rgba( _context, c.red(), c.green(), c.blue(), c.alpha() );
	cairo_set_line_width( _context, p.get_stroke_width() );
}

////////////////////////////////////////

}
