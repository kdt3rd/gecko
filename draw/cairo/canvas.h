
#pragma once

#include <cairo/cairo.h>
#include <draw/canvas.h>

namespace cairo
{

////////////////////////////////////////

class canvas : public draw::canvas
{
public:
	canvas( cairo_surface_t *surf );
	virtual ~canvas( void );

	virtual void fill( const draw::paint &c );
	virtual void draw_path( const std::shared_ptr<draw::path> &p, const draw::paint &c );

	virtual void present( void );

	void set_size( int w, int h );
private:
	void set_cairo_source( const draw::paint &p );

	cairo_surface_t *_surface;
	cairo_t *_context;
};

////////////////////////////////////////

}

