
#include "font.h"

#include <script/extents.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H
#include FT_GLYPH_H

namespace {

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H

}

namespace script { namespace fontconfig
{

////////////////////////////////////////

font::font( FT_Face face, std::string fam, std::string style, double pixsize )
		: script::font( std::move( fam ), std::move( style ), pixsize ),
		  _face( face )
{
	auto err = FT_Select_Charmap( _face, FT_ENCODING_UNICODE );
	if ( err )
	{
		std::cerr << "ERROR selecting UNICODE charmap: [" << FT_Errors[err].code << "] " << FT_Errors[err].message << std::endl;
		if ( _face->charmaps )
		{
			err = FT_Set_Charmap( _face, _face->charmaps[0] );
			if ( err )
				throw std::runtime_error( errorstr( err ) );
		}
		else
			throw std::runtime_error( "Unable to select any character map" );
	}

	std::cout << "Need to add global DPI accessor somehow (multiple screens?)" << std::endl;
	static const int theDPI = 95;

	if ( FT_IS_SCALABLE( _face ) )
	{
		err = FT_Set_Char_Size( _face, static_cast<int>( pixsize * 64.0 ), 0,
								theDPI, theDPI );
		if ( err )
			throw std::runtime_error( "Unable to set character size" );
	}
	else if ( _face->num_fixed_sizes > 1 )
	{
		int targsize = static_cast<int>( pixsize );
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
			else if ( _face->available_sizes[i].width > targsize &&
					  bestSize[0] > _face->available_sizes[i].width )
			{
				bestSize[0] = _face->available_sizes[i].width;
				bestSize[1] = _face->available_sizes[i].height;
			}
		}

		if ( i == _face->num_fixed_sizes )
			_size = bestSize[0];

		// otherwise we have to use FT_Set_Pixel_Sizes
		err = FT_Set_Pixel_Sizes( _face, bestSize[0], bestSize[1] );
		if ( err )
			throw std::runtime_error( "Unable to set fixed character size" );
	}

	_extents.ascent = static_cast<double>( _face->size->metrics.ascender ) / 64.0;
	_extents.descent = static_cast<double>( _face->size->metrics.descender ) / 64.0;

	if ( FT_IS_SCALABLE( _face ) )
	{
		double scaleX = ( static_cast<double>( _face->size->metrics.x_ppem ) /
						  static_cast<double>( _face->units_per_EM ) );
		double scaleY = ( static_cast<double>( _face->size->metrics.y_ppem ) /
						  static_cast<double>( _face->units_per_EM ) );
		_extents.width = std::ceil( static_cast<double>( _face->bbox.xMax - _face->bbox.xMin ) * scaleX );
		_extents.height = static_cast<double>( _face->size->metrics.height ) / 64.0;
			// was: std::ceil( static_cast<double>( _face->bbox.yMax - _face->bbox.yMin ) * scaleY );

		_extents.max_x_advance = static_cast<double>( _face->max_advance_width ) * scaleX;
		_extents.max_y_advance = static_cast<double>( _face->max_advance_height ) * scaleY;
	}
	else
	{
		_extents.width = static_cast<double>( _face->size->metrics.max_advance ) / 64.0;
		_extents.height = static_cast<double>( _face->size->metrics.height ) / 64.0;

		_extents.max_x_advance = static_cast<double>( _face->size->metrics.max_advance ) / 64.0;
		_extents.max_y_advance = 0.0;
	}

}

////////////////////////////////////////

font::~font( void )
{
//	FT_Done_Face( _face );
}

////////////////////////////////////////

double
font::kerning( char32_t c1, char32_t c2 )
{
	FT_Vector kerning;
	FT_UInt prev_index = FT_Get_Char_Index( _face, c1 );
	FT_UInt next_index = FT_Get_Char_Index( _face, c2 );

	FT_Get_Kerning( _face, prev_index, next_index, FT_KERNING_UNFITTED, &kerning );

	return kerning.x / ( 64.0 * 64.0 );
}

////////////////////////////////////////

const char *
font::errorstr( FT_Error err )
{
	return FT_Errors[err].message;
}

////////////////////////////////////////

const text_extents &
font::get_glyph( char32_t char_code )
{
	auto i = _glyph_cache.find( char_code );
	if ( i == _glyph_cache.end() )
	{
		// We are only loading a grayscale bitmap for now. Could
		// set the LCD filtering and load a color bitmap
		// also, we could switch to the signed distance field stuff
		// fairly easily if we added the computation here and used
		// the appropriate shader...
		FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT;
		FT_UInt index = FT_Get_Char_Index( _face, char_code );

		auto err = FT_Load_Glyph( _face, index, flags );
		if ( err )
			throw std::runtime_error( "Unable to load glyph" );

		FT_GlyphSlot slot = _face->glyph;

        int w = slot->bitmap.width;
        int h = slot->bitmap.rows;

		if ( w > 0 && h > 0 )
		{
			add_glyph( glData, glPitch, w, h );
		}

		text_extents &gle = _glyph_cache[char_code];

//		err = FT_Load_Glyph( _face, nglyph->index(), FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_HINTING );

		gle.x_bearing = static_cast<double>( slot->metrics.horiBearingX ) / 64.0;
		gle.y_bearing = static_cast<double>( slot->metrics.horiBearingY ) / 64.0;
		gle.width = static_cast<double>( slot->metrics.width ) / 64.0;
		gle.height = static_cast<double>( slot->metrics.height ) / 64.0;
		gle.x_advance = static_cast<double>( slot->metrics.horiAdvance ) / 64.0;
		gle.y_advance = static_cast<double>( slot->metrics.vertAdvance ) / 64.0;

		return gle;
	}

	return i->second;
}

////////////////////////////////////////

} }

