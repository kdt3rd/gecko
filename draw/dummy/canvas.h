
#pragma once

#include <draw/canvas.h>

namespace dummy
{

////////////////////////////////////////

/// @brief Dummy implementation of canvas.
class canvas : public draw::canvas
{
public:
	canvas( void );
	virtual ~canvas( void );

	virtual void fill( const draw::paint &c );
	virtual void fill( const draw::rect &r, const draw::paint &c );

	virtual void draw_path( const draw::path &p, const draw::paint &c );

	virtual void draw_text( const std::shared_ptr<draw::font> &font, const draw::point &p, const std::string &utf8, const draw::paint &c );

	virtual void present( void );

	virtual void screenshot_png( const char *filename );
};

////////////////////////////////////////

}

