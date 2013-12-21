
#pragma once

#include <cairo.h>
#include <draw/canvas.h>

namespace cairo
{

////////////////////////////////////////

/// @brief Cairo implementation of canvas.
class canvas : public draw::canvas
{
public:
	canvas( void );
	virtual ~canvas( void );

	/// @brief Get the cairo surface.
	cairo_surface_t *get_surface( void ) const { return _surface; }

	/// @brief Check if the canvas has a cairo surface.
	bool has_surface( void ) const { return _surface != nullptr; }

	/// @brief Set the cairo surface.
	void set_surface( cairo_surface_t *surf );

	/// @brief Clear the cairo surface.
	void clear_surface( void );

	virtual void fill( const draw::paint &c );
	virtual void fill( const draw::rect &r, const draw::paint &c );

	virtual void draw_path( const draw::path &p, const draw::paint &c );

	virtual void draw_text( const std::shared_ptr<draw::font> &font, const draw::point &p, const std::string &utf8, const draw::paint &c );

	virtual void clip( const draw::rect &r );

	virtual void translate( double x, double y );

	virtual void present( void );

	virtual void save( void );
	virtual void restore( void );

	virtual void screenshot_png( const char *filename );

private:
	void set_cairo( const draw::paint &p );
	void set_cairo_stroke( const draw::paint &p );
	bool set_cairo_fill( const draw::paint &p );
	void set_cairo_font( const std::shared_ptr<draw::font> &font );

	void check_error( void );

	cairo_surface_t *_surface = nullptr;
	cairo_t *_context = nullptr;
};

////////////////////////////////////////

}

