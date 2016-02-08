
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <base/point.h>
#include <base/pack.h>
#include "extents.h"

namespace script
{

////////////////////////////////////////

/// @brief Font.
/// Fonts can be loaded and queried at any time during the setup / drawing
/// cycle. See the @sa glyph_version function below to know whether the
/// consumer needs to update it's textures
class font
{
public:
	font( std::string fam, std::string sty, double sz );
	virtual ~font( void );

	inline const std::string &family( void ) const { return _family; }
	inline const std::string &style( void ) const { return _style; }
	inline double size( void ) const { return _size; }

	/// @brief Accessor to check if triplet of values is this font.
	inline bool matches( const std::string &f, const std::string &s, double sz ) const
	{
		return family() == f && style() == s && std::abs( size() - sz ) < 0.000001;
	}

	/// @brief The general size of the font.
	/// The general distances needed to properly position and align text using this font.
	/// @return The font extents.
	inline const font_extents &extents( void ) const { return _extents; }

	/// @brief Retrieves the extents for a single glyph.
	const text_extents &extents( char32_t charcode )
	{
		return get_glyph( charcode );
	}

	/// @brief The specific size of the text
	/// The specific distances and dimensions of the text when drawn with the given font.
	/// NB: This assumes a horizontal layout currently.
	///
	/// @param utf8 The utf8 string to measure.
	/// @return The text extents.
	text_extents extents( const std::string &utf8 );

	/// @brief Get kerning between two glyphs.
	virtual double kerning( char32_t c1, char32_t c2 ) = 0;

	/// @brief Renders the given text.
	/// Given the start as the origin for the baseline, 'renders' the
	/// given text by filling a set of output coordinates and vertex
	/// indices to render.
	void render( std::vector<float> &outCoords,
				 std::vector<float> &texCoords,
				 std::vector<uint16_t> &renderIndices,
				 const base::point &start, const std::string &utf8 );

	/// @brief Glyph version.
	/// Increments every time a glyph is loaded into the internal
	/// store.  This is done such that the font only deals with CPU
	/// ram and knows nothing about textures or the current drawing
	/// context / canvas, which enables fonts to be loaded and queried
	/// when the context may not be current.
	uint32_t glyph_version( void ) const { return _glyph_version; }

	int bitmap_width( void ) const { return _glyph_pack.width(); }
	int bitmap_height( void ) const { return _glyph_pack.height(); }

	const std::vector<uint8_t> &bitmap( void ) const { return _glyph_bitmap; }
//	const std::vector<float> &glyph_coords( void ) const { return _glyph_coords; }

	void load_glyph( char32_t cc )
	{
		(void)get_glyph( cc );
	}

protected:
	void add_glyph( char32_t char_code, const uint8_t *glData, int glPitch, int w, int h );

	/// @brief Retrieves a glyph given a unicode char code.
	/// Function should load all attributes about the glyph and put
	/// them into the glyph cache.
	///
	/// @param char_code Unicode character to find
	/// @return reference to glyph in glyph cache
	virtual const text_extents &get_glyph( char32_t char_code ) = 0;

	void bump_glyph_store_size( void );

	font_extents _extents;

	uint32_t _glyph_version = 0;
	base::pack _glyph_pack;
	std::vector<uint8_t> _glyph_bitmap;

	std::vector<float> _glyph_coords;

	std::map<char32_t, size_t> _glyph_index_offset;
	std::map<char32_t, text_extents> _glyph_cache;

	std::string _family;
	std::string _style;
	double _size;
};

////////////////////////////////////////

}
