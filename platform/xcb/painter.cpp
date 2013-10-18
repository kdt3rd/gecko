
#include <vector>
#include <stdexcept>
#include <core/contract.h>
#include "painter.h"
#include "rectangles.h"
#include "points.h"

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

	const uint32_t values[] = { col->pixel, col->pixel };
	xcb_change_gc( _connection, _context, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values );
	xcb_change_window_attributes( _connection, _win, XCB_CW_BACK_PIXEL, values );
}

////////////////////////////////////////

std::shared_ptr<platform::points> painter::new_points( void )
{
	return std::make_shared<points>();
}


////////////////////////////////////////

void painter::draw_points( const std::shared_ptr<platform::points> &ps )
{
	auto tmp = std::dynamic_pointer_cast<points>( ps );
	if ( tmp )
	{
		const std::vector<xcb_point_t> &points = tmp->get_list();
		xcb_poly_point( _connection, XCB_COORD_MODE_ORIGIN, _win, _context, points.size(), points.data() );
	}
	else
		throw std::runtime_error( "invalid points" );
}

////////////////////////////////////////

void painter::draw_polygon( const std::shared_ptr<platform::points> &ps )
{
	auto tmp = std::dynamic_pointer_cast<points>( ps );
	if ( tmp )
	{
		const std::vector<xcb_point_t> &points = tmp->get_list();
		xcb_poly_line( _connection, XCB_COORD_MODE_ORIGIN, _win, _context, points.size(), points.data() );
	}
	else
		throw std::runtime_error( "invalid points" );
}

////////////////////////////////////////

void painter::draw_lines( const line *ls, size_t ln )
{
}

////////////////////////////////////////

std::shared_ptr<platform::rectangles> painter::new_rectangles( void )
{
	return std::make_shared<rectangles>();
}

////////////////////////////////////////

void painter::draw_rects( const std::shared_ptr<platform::rectangles> &rs )
{
	auto tmp = std::dynamic_pointer_cast<rectangles>( rs );
	if ( tmp )
	{
		const std::vector<xcb_rectangle_t> &rects = tmp->get_list();
		xcb_poly_rectangle( _connection, _win, _context, rects.size(), rects.data() );
	}
	else
		throw std::runtime_error( "invalid rectangles" );
}

////////////////////////////////////////

void painter::fill_rects( const std::shared_ptr<platform::rectangles> &rs )
{
	auto tmp = std::dynamic_pointer_cast<rectangles>( rs );
	if ( tmp )
	{
		const std::vector<xcb_rectangle_t> &rects = tmp->get_list();
		xcb_poly_fill_rectangle( _connection, _win, _context, rects.size(), rects.data() );
	}
	else
		throw std::runtime_error( "invalid rectangles" );
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
