// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <script/font.h>

////////////////////////////////////////

namespace script
{
namespace cocoa
{
/// @brief Font subclass for cocoa.
class font : public ::script::font
{
public:
    font( void *font, std::string fam, std::string style, points pts );
    ~font( void ) override;

    void init_font( void ) override;

    points kerning( char32_t c1, char32_t c2 ) override;

protected:
    const text_extents &get_glyph( char32_t char_code ) override;

private:
    void * _font;
    float _scalePixToPointsHoriz = 1.f;
    float _scalePixToPointsVert  = 1.f;
};

} // namespace cocoa
} // namespace script
