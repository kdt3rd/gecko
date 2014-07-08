
#pragma once

#include <script/font.h>


////////////////////////////////////////


namespace script { namespace cocoa
{

/// @brief Font subclass for cocoa.
class font : public ::script::font
{
public:
	font( void *font, std::string fam, std::string style, double pixsize );
	~font( void );

	double kerning( char32_t c1, char32_t c2 ) override;

protected:
	const text_extents &get_glyph( char32_t char_code ) override;

private:
	void *_font;
};

} }

