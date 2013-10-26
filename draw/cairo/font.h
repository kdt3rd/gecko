
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <cairo.h>
#include <draw/font.h>

namespace cairo
{

////////////////////////////////////////

class font : public draw::font
{
public:
	font( FT_Library &lib, const std::string &filename, int font_idx );
	virtual ~font( void );

	std::string family( void ) const { return std::string( _ft_face->family_name ); }
	std::string style( void ) const { return std::string( _ft_face->style_name ); }

	bool is_scalable( void ) const { return FT_IS_SCALABLE( _ft_face ); }
	bool is_fixed_width( void ) const { return FT_IS_FIXED_WIDTH( _ft_face ); }

	cairo_font_face_t *cairo_font( void ) const { return _font; }

private:
	cairo_font_face_t *_font = nullptr;
	FT_Face _ft_face;
};

////////////////////////////////////////

}

