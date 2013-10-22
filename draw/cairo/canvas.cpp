
#include "canvas.h"
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
}

////////////////////////////////////////

void canvas::fill( const draw::paint &c )
{
	set_cairo_source( c );
	cairo_fill( _context );
}

////////////////////////////////////////

void canvas::draw_path( const std::shared_ptr<draw::path> &p, const draw::paint &c )
{
	set_cairo_source( c );
}

////////////////////////////////////////

void canvas::set_cairo_source( const draw::paint &p )
{
	const draw::color &c = p.get_color();
	cairo_set_source_rgba( _context, c.red(), c.green(), c.blue(), c.alpha() );
}

////////////////////////////////////////

}
