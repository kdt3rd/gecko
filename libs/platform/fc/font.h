
#pragma once

#include <draw/font.h>

#include <ft2build.h>
#include FT_FREETYPE_H

////////////////////////////////////////


namespace platform { namespace fc
{

/// @brief Font subclass for fontconfig / freetype.
/// @author Kimball Thurston
class font : public ::draw::font
{
public:
	font( FT_Face face, std::string fam, std::string style, double pixsize );
	virtual ~font( void );

	static const char *errorstr( FT_Error err );

protected:
	const draw::glyph &get_glyph( wchar_t char_code ) override;

private:
	FT_Face _face;
};

} }

