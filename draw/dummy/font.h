
#pragma once

#include <draw/font.h>

namespace dummy
{

////////////////////////////////////////

/// @brief Dummy implementation of font.
class font : public draw::font
{
public:
	font( std::string fam, std::string sty, double sz );
	virtual ~font( void );

	virtual draw::font_extents extents( void );
	virtual draw::text_extents text_extents( const std::string &utf8 );
};

////////////////////////////////////////

}

