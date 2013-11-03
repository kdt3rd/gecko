
#pragma once

#include <cairo.h>
#include <draw/font.h>

namespace cairo
{

////////////////////////////////////////

/// @brief Cairo implementation of font.
class font : public draw::font
{
public:
	font( cairo_font_face_t *face, std::string fam, std::string sty, double sz );
	virtual ~font( void );

	virtual draw::font_extents extents( void );
	virtual draw::text_extents text_extents( const std::string &utf8 );

	cairo_scaled_font_t *cairo_font( void ) const { return _font; }

private:
	cairo_scaled_font_t *_font = nullptr;
};

////////////////////////////////////////

}

