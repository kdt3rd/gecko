//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <script/font.h>

#pragma GCC diagnostic push
#if defined(__clang__)
# pragma GCC diagnostic ignored "-Wreserved-id-macro"
# pragma GCC diagnostic ignored "-Wdocumentation"
#endif
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma GCC diagnostic pop

////////////////////////////////////////


namespace script { namespace freetype2
{

/// @brief Font subclass for freetype.
/// @author Kimball Thurston
class font : public ::script::font
{
public:
	font( FT_Face face, std::string fam, std::string style, points pts, const std::shared_ptr<uint8_t []> &ttfData = std::shared_ptr<uint8_t []>() );
	~font( void ) override;

	void init_font( void ) override;

	extent_type kerning( char32_t c1, char32_t c2 ) override;

	static const char *errorstr( FT_Error err );

protected:
	const text_extents &get_glyph( char32_t char_code ) override;

private:
	FT_Face _face;
	std::shared_ptr<uint8_t []> _font_data;
};

} // namespace freetype2
} // namespace script
