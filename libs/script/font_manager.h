//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <set>
#include <memory>
#include "font.h"

namespace script
{

////////////////////////////////////////

/// @brief Font manager
///
/// Allows creation and listing of fonts.
class font_manager
{
public:
	/// @brief Constructor.
	font_manager( void );

	/// @brief Destructor.
	virtual ~font_manager( void );

	void load_dpi( int h, int v ) { _dpi_h = h; _dpi_v = v; }
	void max_glyph_store( int w, int h ) { _max_glyph_w = w; _max_glyph_h = h; }

	/// @brief List of families.
	///
	/// Get a list of families of all managed fonts.
	/// @return The list of families
	virtual std::set<std::string> get_families( void ) = 0;

	/// @brief List of style.
	///
	/// Get a list of styles available.
	/// @return The list of styles.
	virtual std::set<std::string> get_styles( const std::string &family ) = 0;

	/// @brief Get a particular font.
	///
	/// Create and return a font with the given attributes.
	/// @param family Family of the font to create
	/// @param style Style of the font to create
	/// @param pixsize Size of the font to create
	///
	/// @return The best matching font found
	virtual std::shared_ptr<font> get_font( const std::string &family, const std::string &style, double pixsize ) = 0;

	static const std::vector<std::shared_ptr<font_manager>> &list( void );

	static std::shared_ptr<font_manager> common( void );

protected:
	int _max_glyph_w = 2048;
	int _max_glyph_h = 2048;
	int _dpi_h = 95;
	int _dpi_v = 95;

private:
	static void init( void );
	static void enroll( const std::shared_ptr<font_manager> &mgr ); 
};

////////////////////////////////////////

}

