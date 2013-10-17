
#include <vector>
#include <stdexcept>
#include <core/contract.h>
#include "painter.h"

namespace xcb
{

////////////////////////////////////////

painter::painter( xcb_connection_t *c )
	: _connection( c )
{
	precondition( _connection, "null connection" );
}

////////////////////////////////////////

painter::~painter( void )
{
}

////////////////////////////////////////

void painter::set_color( const color &c )
{
}

////////////////////////////////////////

void painter::draw_points( const point *ps, size_t pn )
{
}

////////////////////////////////////////

void painter::draw_polygon( const point *ps, size_t pn )
{
}

////////////////////////////////////////

void painter::draw_lines( const line *ls, size_t ln )
{
}

////////////////////////////////////////

void painter::draw_rects( const rectangle *rs, size_t rn )
{
}

////////////////////////////////////////

void painter::clear( void )
{
}

////////////////////////////////////////

void painter::present( void )
{
	precondition( _connection, "null connection" );
	xcb_flush( _connection );
}

////////////////////////////////////////

}
