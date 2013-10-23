
#include "canvas.h"
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

void canvas::draw_path( const std::shared_ptr<draw::path> &p, const draw::paint &c )
{
	set_cairo_source( c );
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
}

////////////////////////////////////////

}
