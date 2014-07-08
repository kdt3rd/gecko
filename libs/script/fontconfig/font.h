
#pragma once

#include <script/font.h>

#include <ft2build.h>
#include FT_FREETYPE_H

////////////////////////////////////////


namespace script { namespace fontconfig
{

/// @brief Font subclass for freetype.
/// @author Kimball Thurston
class font : public ::script::font
{
public:
	font( FT_Face face, std::string fam, std::string style, double pixsize );
	~font( void );

	double kerning( char32_t c1, char32_t c2 ) override;

	static const char *errorstr( FT_Error err );

protected:
	const text_extents &get_glyph( char32_t char_code ) override;

private:
	FT_Face _face;
};

} }

