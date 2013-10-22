
#pragma once

#include <platform/painter.h>
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

class painter : public platform::painter
{
public:
	painter( xcb_connection_t *c, xcb_screen_t *screen, xcb_window_t win );
	virtual ~painter( void );

	virtual void set_color( const draw::color &c );

	virtual std::shared_ptr<platform::points> new_points( void );

	virtual void draw_points( const std::shared_ptr<platform::points> &ps );

	virtual void draw_polygon( const std::shared_ptr<platform::points> &ps );
	virtual void fill_polygon( const std::shared_ptr<platform::points> &ps );

//	virtual void draw_lines( const line *ps, size_t pn );

	virtual std::shared_ptr<platform::rectangles> new_rectangles( void );
	virtual void draw_rects( const std::shared_ptr<platform::rectangles> &rs );
	virtual void fill_rects( const std::shared_ptr<platform::rectangles> &rs );

	virtual void clear( void );

	virtual void present( void );

private:
	xcb_connection_t *_connection;
	xcb_window_t _win;
	xcb_colormap_t _colormap;
	xcb_gcontext_t _context;
};

////////////////////////////////////////

}

