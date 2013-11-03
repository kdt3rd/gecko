
#include "font.h"
#include <stdexcept>

namespace dummy
{

////////////////////////////////////////

font::font( std::string fam, std::string sty, double sz )
	: draw::font( std::move(fam), std::move(sty), sz )
{
}

////////////////////////////////////////

font::~font( void )
{
}

////////////////////////////////////////

draw::font_extents font::extents( void )
{
	return { 0.0, 0.0, 0.0, 0.0, 0.0 };
}

////////////////////////////////////////

draw::text_extents font::text_extents( const std::string &utf8 )
{
	return { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
}

////////////////////////////////////////

}

