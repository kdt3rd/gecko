// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "font.h"

#include <script/extents.h>

// meh, just disable all these warnings for this file since freetype is C based
#if defined( __clang__ )
#    pragma GCC diagnostic ignored "-Wreserved-id-macro"
#    pragma GCC diagnostic ignored "-Wdocumentation"
#endif
#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H
#include FT_GLYPH_H

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s ) { e, s },
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST                                                      \
    {                                                                          \
        0, 0                                                                   \
    }                                                                          \
    }                                                                          \
    ;

namespace
{
const struct
{
    int         code;
    const char *message;
} FT_Errors[] =
#include FT_ERRORS_H

} // namespace

namespace script
{
namespace freetype2
{
////////////////////////////////////////

font::font(
    FT_Face                           face,
    std::string                       fam,
    std::string                       style,
    base::units::points<extent_type>  pts,
    const std::shared_ptr<uint8_t[]> &ttfData )
    : script::font( std::move( fam ), std::move( style ), pts )
    , _face( face )
    , _font_data( ttfData )
{}

////////////////////////////////////////

void font::init_font( void )
{
    auto err = FT_Select_Charmap( _face, FT_ENCODING_UNICODE );
    if ( err )
    {
        std::cerr << "ERROR selecting UNICODE charmap: [" << FT_Errors[err].code
                  << "] " << FT_Errors[err].message << std::endl;
        if ( _face->charmaps )
        {
            err = FT_Set_Charmap( _face, _face->charmaps[0] );
            if ( err )
                throw std::runtime_error( errorstr( err ) );
        }
        else
            throw std::runtime_error( "Unable to select any character map" );
    }

    _scalePixelsToPointsHoriz = 72.0 / ( 64.0 * static_cast<double>( _dpi_h ) );
    _scalePixelsToPointsVert  = 72.0 / ( 64.0 * static_cast<double>( _dpi_v ) );

    if ( FT_IS_SCALABLE( _face ) )
    {
        //		std::cout << "Scalable font, setting to " << _size << " points (" << int( _size.count() * 64.F )
        //				  << " 26.6) hres " << _dpi_h << " vres " << _dpi_v << std::endl;
        err = FT_Set_Char_Size(
            _face,
            static_cast<int>( _size.count() * 64.F ),
            0,
            _dpi_h,
            _dpi_v );
        if ( err )
            throw std::runtime_error( "Unable to set character size" );

        // size values in units
        _scaleUnitsToPointsHoriz =
            ( static_cast<double>( _face->size->metrics.x_ppem ) * 72.0 /
              ( static_cast<double>( _face->units_per_EM ) *
                static_cast<double>( _dpi_h ) ) );
        _scaleUnitsToPointsVert =
            ( static_cast<double>( _face->size->metrics.y_ppem ) * 72.0 /
              ( static_cast<double>( _face->units_per_EM ) *
                static_cast<double>( _dpi_v ) ) );
        _extents.ascent =
            static_cast<double>( _face->ascender ) * _scaleUnitsToPointsVert;
        _extents.descent =
            static_cast<double>( _face->descender ) * _scaleUnitsToPointsVert;
        _extents.width =
            static_cast<double>( _face->bbox.xMax - _face->bbox.xMin ) *
            _scaleUnitsToPointsHoriz;
        //_extents.height = static_cast<double>( _face->bbox.yMax - _face->bbox.yMin ) * _scaleUnitsToPointsVert;
        _extents.height = _extents.ascent - _extents.descent;
        _extents.max_x_advance =
            static_cast<double>( _face->max_advance_width ) *
            _scaleUnitsToPointsHoriz;
        _extents.max_y_advance =
            static_cast<double>( _face->max_advance_height ) *
            _scaleUnitsToPointsVert;
    }
    else if ( _face->num_fixed_sizes > 1 )
    {
        int targsize = static_cast<int>( _size.count() );
        int bestSize[2];
        bestSize[0] = bestSize[1] = targsize;
        int i;
        for ( i = 0; i != _face->num_fixed_sizes; ++i )
        {
            if ( _face->available_sizes[i].width == targsize )
            {
                bestSize[1] = _face->available_sizes[i].height;
                break;
            }
            else if (
                _face->available_sizes[i].width > targsize &&
                bestSize[0] > _face->available_sizes[i].width )
            {
                bestSize[0] = _face->available_sizes[i].width;
                bestSize[1] = _face->available_sizes[i].height;
            }
        }

        if ( i == _face->num_fixed_sizes )
            _size = bestSize[0];

        // otherwise we have to use FT_Set_Pixel_Sizes
        err = FT_Set_Pixel_Sizes(
            _face,
            static_cast<FT_UInt>( bestSize[0] ),
            static_cast<FT_UInt>( bestSize[1] ) );
        if ( err )
            throw std::runtime_error( "Unable to set fixed character size" );

        _extents.ascent  = 0;
        _extents.descent = 0;
        _extents.width =
            static_cast<double>( _face->size->metrics.max_advance ) *
            _scalePixelsToPointsHoriz;
        _extents.height = static_cast<double>( _face->size->metrics.height ) *
                          _scalePixelsToPointsVert;
        _extents.max_x_advance = _extents.width;
        _extents.max_y_advance = _extents.height;
    }

    //	std::cout << "font has width " << _extents.width << " height " << _extents.height << " ascent " << _extents.ascent << " desc " << _extents.descent << " max_x " << _extents.max_x_advance << " max_y " << _extents.max_y_advance << std::endl;
}

////////////////////////////////////////

font::~font( void )
{
    // the destruction of the library instance will clean up the face...
    //	FT_Done_Face( _face );
}

////////////////////////////////////////

points font::kerning( char32_t c1, char32_t c2 )
{
    FT_Vector kerning;
    FT_UInt   prev_index = FT_Get_Char_Index( _face, c1 );
    FT_UInt   next_index = FT_Get_Char_Index( _face, c2 );

    FT_Get_Kerning(
        _face, prev_index, next_index, FT_KERNING_UNSCALED, &kerning );

    // TODO: know about vertical layout
    return static_cast<double>( kerning.x ) * _scaleUnitsToPointsHoriz;
}

////////////////////////////////////////

const char *font::errorstr( FT_Error err ) { return FT_Errors[err].message; }

////////////////////////////////////////

const text_extents &font::get_glyph( char32_t char_code )
{
    auto i = _glyph_cache.find( char_code );
    if ( i == _glyph_cache.end() )
    {
        // We are only loading a grayscale bitmap for now. Could
        // set the LCD filtering and load a color bitmap
        // also, we could switch to the signed distance field stuff
        // fairly easily if we added the computation here and used
        // the appropriate shader...
        FT_Int32 flags =
            FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER; // | FT_LOAD_FORCE_AUTOHINT;
        FT_UInt index = FT_Get_Char_Index( _face, char_code );

        auto err = FT_Load_Glyph( _face, index, flags );
        if ( err )
            throw std::runtime_error( "Unable to load glyph" );

        FT_GlyphSlot slot = _face->glyph;

        int w = static_cast<int>( slot->bitmap.width );
        int h = static_cast<int>( slot->bitmap.rows );

        if ( w > 0 && h > 0 )
        {
            const uint8_t *glData = slot->bitmap.buffer;
            add_glyph( char_code, glData, slot->bitmap.pitch, w, h );
        }

        text_extents &gle = _glyph_cache[char_code];

        //		err = FT_Load_Glyph( _face, nglyph->index(), FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_HINTING );

        gle.x_bearing = static_cast<double>( slot->metrics.horiBearingX ) *
                        _scalePixelsToPointsHoriz;
        gle.y_bearing = static_cast<double>( slot->metrics.horiBearingY ) *
                        _scalePixelsToPointsVert;
        gle.width = static_cast<double>( slot->metrics.width ) *
                    _scalePixelsToPointsHoriz;
        gle.height = static_cast<double>( slot->metrics.height ) *
                     _scalePixelsToPointsVert;
        gle.x_advance = static_cast<double>( slot->metrics.horiAdvance ) *
                        _scalePixelsToPointsHoriz;
        gle.y_advance = static_cast<double>( slot->metrics.vertAdvance ) *
                        _scalePixelsToPointsVert;

        return gle;
    }

    return i->second;
}

////////////////////////////////////////

} // namespace freetype2
} // namespace script
