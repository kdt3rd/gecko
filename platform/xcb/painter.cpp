
#include <vector>
#include <stdexcept>
#include <core/contract.h>
#include "painter.h"

namespace xcb
{

////////////////////////////////////////

painter::painter( xcb_connection_t *c, xcb_screen_t *screen, xcb_window_t win )
	: _connection( c ), _win( win )
{
	precondition( _connection, "null connection" );

	_context = xcb_generate_id( _connection );
	_colormap = screen->default_colormap;

	const uint32_t values[] = { screen->black_pixel };
	xcb_create_gc( _connection, _context, win, XCB_GC_FOREGROUND, values );
}

////////////////////////////////////////

painter::~painter( void )
{
}

////////////////////////////////////////

void painter::set_color( const color &c )
{
	auto reply = xcb_alloc_color( _connection, _colormap, c.red() * 65535 + 0.5, c.green() * 65535 + 0.5, c.blue() * 65535 + 0.5 );
	xcb_alloc_color_reply_t *col = xcb_alloc_color_reply( _connection, reply, NULL );

	const uint32_t values[] = { col->pixel };
	xcb_change_gc( _connection, _context, XCB_GC_FOREGROUND, values );
}

////////////////////////////////////////

void painter::draw_points( const point *ps, size_t pn )
{
	std::vector<xcb_point_t> points( pn );
	for ( size_t i = 0; i < pn; ++i )
	{
		points[i].x = ps[i].x();
		points[i].y = ps[i].y();
	}

	xcb_poly_point( _connection, XCB_COORD_MODE_ORIGIN, _win, _context, points.size(), points.data() );
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
	std::vector<xcb_rectangle_t> rects( rn );
	for ( size_t i = 0; i < rn; ++i )
	{
		rects[i].x = rs[i].x1();
		rects[i].y = rs[i].y1();
		rects[i].width = rs[i].width() - 1;
		rects[i].height = rs[i].height() - 1;
	}

	xcb_poly_rectangle( _connection, _win, _context, rects.size(), rects.data() );
}

////////////////////////////////////////

void painter::clear( void )
{
	xcb_clear_area( _connection, 0, _win, 0, 0, 0, 0 );
}

////////////////////////////////////////

void painter::present( void )
{
	precondition( _connection, "null connection" );
	xcb_flush( _connection );
}

////////////////////////////////////////

}
