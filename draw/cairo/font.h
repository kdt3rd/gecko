
#pragma once

#include <cairo.h>
#include <draw/font.h>

namespace cairo
{

////////////////////////////////////////

class font : public draw::font
{
public:
	font( cairo_font_face_t *face, std::string fam, std::string sty, double sz );
	virtual ~font( void );

	cairo_font_face_t *cairo_font( void ) const { return _font; }

private:
	cairo_font_face_t *_font = nullptr;
};

////////////////////////////////////////

}

