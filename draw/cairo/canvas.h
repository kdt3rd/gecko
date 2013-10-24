
#pragma once

#include <cairo/cairo.h>
#include <draw/canvas.h>

namespace cairo
{

////////////////////////////////////////

class canvas : public draw::canvas
{
public:
	canvas( void );
	virtual ~canvas( void );

	bool has_surface( void ) const { return bool(_surface); }
	void set_surface( cairo_surface_t *surf );
	void clear_surface( void );
	void set_size( int w, int h );

	virtual void fill( const draw::paint &c );

	virtual void draw_path( const draw::path &p, const draw::paint &c );

	virtual void present( void );


private:
	void set_cairo_source( const draw::paint &p );

	void check_error( void );

	cairo_surface_t *_surface = nullptr;
	cairo_t *_context = nullptr;
};

////////////////////////////////////////

}

