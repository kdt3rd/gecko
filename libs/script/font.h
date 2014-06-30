
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <base/point.h>
#include <base/pack.h>
#include "extents.h"
#include "glyph.h"

namespace script
{

////////////////////////////////////////

/// @brief Font
///
/// Fonts can be loaded and queried at any time during the setup / drawing
/// cycle. See the @fun glyph_version function below to know whether the
/// consumer needs to update it's textures
class font
{
public:
	font( std::string fam, std::string sty, double sz );
	virtual ~font( void );

	inline const std::string &family( void ) const { return _family; }
	inline const std::string &style( void ) const { return _style; }
	inline double size( void ) const { return _size; }

	/// @brief accessor to check if triplet of values is this font
	inline bool matches( const std::string &f, const std::string &s, double sz ) const
	{
		return family() == f && style() == s && size() == sz;
	}

	/// @brief The general size of the font
	/// The general distances needed to properly position and align text using this font.
	/// @param font Font to measure
	/// @return The font extents
	inline const font_extents &extents( void ) const { return _extents; }

	/// Retrieves the extents for a single glyph
	const text_extents &extents( wchar_t charcode ) { return get_glyph( charcode ).extents(); }

	/// @brief The specific size of the text
	/// The specific distances and dimensions of the text when drawn with the given font.
	///
	/// NB: This assumes a horizontal layout currently
	/// 
	/// @param utf8 The utf8 string to measure
	/// @return The text extents
	text_extents extents( const std::string &utf8 );

	/// @brief Renders the given text
	/// 
	/// Given the start as the origin for the baseline, 'renders' the
	/// given text by filling a set of output coordinates and vertex
	/// indices to render.
	///
	void render( std::vector<float> &outCoords,
				 std::vector<float> &texCoords,
				 std::vector<uint16_t> &renderIndices,
				 const base::point &start, const std::string &utf8 );

	/// Increments every time a glyph is loaded into the internal
	/// store.  This is done such that the font only deals with CPU
	/// ram and knows nothing about textures or the current drawing
	/// context / canvas, which enables fonts to be loaded and queried
	/// when the context may not be current.
	///
	/// The canvas
	uint32_t glyph_version( void ) const { return _glyph_version; }

	int glyph_texture_size_x( void ) const { return _glyph_pack.width(); }
	int glyph_texture_size_y( void ) const { return _glyph_pack.height(); }
	const std::vector<uint8_t> &glyph_texture( void ) const { return _glyph_bitmap; }
	const std::vector<float> &glyph_coords( void ) const { return _glyph_coords; }
	
protected:
	/// @brief Retrieves a glyph given a unicode char code
	/// Function should load all attributes about the glyph and put
	/// them into the glyph cache
	///
	/// @param char_code Unicode character to find
	/// @return reference to glyph in glyph cache
	virtual const glyph &get_glyph( wchar_t char_code ) = 0;

	void bump_glyph_store_size( void );

	font_extents _extents;

	uint32_t _glyph_version = 0;
	base::pack _glyph_pack;
	std::vector<uint8_t> _glyph_bitmap;
	std::vector<float> _glyph_coords;
	std::map<wchar_t, size_t> _glyph_index_offset;

	std::map<wchar_t, std::shared_ptr<glyph>> _glyph_cache;
	std::map<std::pair<char32_t,char32_t>,double> _kerning;
	
	std::string _family;
	std::string _style;
	double _size;

};

////////////////////////////////////////

}
