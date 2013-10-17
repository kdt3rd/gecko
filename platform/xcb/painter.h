
#pragma once

#include <platform/painter.h>
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

class painter : public platform::painter
{
public:
	painter( xcb_connection_t *c );
	virtual ~painter( void );

	virtual void set_color( const color &c );

	virtual void draw_points( const point *ls, size_t ln );
	virtual void draw_polygon( const point *ps, size_t pn );

	virtual void draw_lines( const line *ps, size_t pn );
	virtual void draw_rects( const rectangle *rs, size_t rn );

	virtual void clear( void );

	virtual void present( void );

private:
	xcb_connection_t *_connection;
};

////////////////////////////////////////

}

