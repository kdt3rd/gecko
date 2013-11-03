
#pragma once

#include <string>
#include "extents.h"

namespace draw
{

////////////////////////////////////////

class font
{
public:
	font( std::string fam, std::string sty, double sz );
	virtual ~font( void );

	std::string family( void ) const { return _family; }
	std::string style( void ) const { return _style; }
	double size( void ) const { return _size; }

	/// @brief The general size of the font
	/// The general distances needed to properly position and align text using this font.
	/// @param font Font to measure
	/// @return The font extents
	virtual font_extents extents( void ) = 0;

	/// @brief The specific size of the text
	/// The specific distances and dimensions of the text when drawn with the given font.
	/// @param font Font to measure
	/// @param utf8 The utf8 string to measure
	/// @return The text extents
	virtual text_extents text_extents( const std::string &utf8 ) = 0;

private:
	std::string _family;
	std::string _style;
	double _size;
};

////////////////////////////////////////

}
