// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <script/font.h>

#pragma GCC diagnostic push
#if defined( __clang__ )
#    pragma GCC diagnostic ignored "-Wreserved-id-macro"
#    pragma GCC diagnostic ignored "-Wdocumentation"
#endif
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma GCC diagnostic pop

////////////////////////////////////////

namespace script
{
namespace freetype2
{
/// @brief Font subclass for freetype.
/// @author Kimball Thurston
class font : public ::script::font
{
public:
    font(
        FT_Face                           face,
        std::string                       fam,
        std::string                       style,
        points                            pts,
        const std::shared_ptr<uint8_t[]> &ttfData =
            std::shared_ptr<uint8_t[]>() );
    ~font( void ) override;

    void init_font( void ) override;

    points kerning( char32_t c1, char32_t c2 ) override;

    static const char *errorstr( FT_Error err );

protected:
    const text_extents &get_glyph( char32_t char_code ) override;

private:
    FT_Face                    _face;
    std::shared_ptr<uint8_t[]> _font_data;
    double                     _scaleUnitsToPointsHoriz  = 1.0;
    double                     _scaleUnitsToPointsVert   = 1.0;
    double                     _scalePixelsToPointsHoriz = 1.0;
    double                     _scalePixelsToPointsVert  = 1.0;
};

} // namespace freetype2
} // namespace script
