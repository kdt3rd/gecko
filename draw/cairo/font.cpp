
#include "font.h"
#include <stdexcept>
#include <cairo-ft.h>

namespace cairo
{

////////////////////////////////////////

font::font( FT_Library &lib, const std::string &filename, int font_idx, double size )
	: _size( size )
{
	auto error = FT_New_Face( lib, filename.c_str(), font_idx, &_ft_face );
	if ( error )
		throw std::runtime_error( "freetype error" );
	_font = cairo_ft_font_face_create_for_ft_face( _ft_face, 0 );
}

////////////////////////////////////////

font::~font( void )
{
	cairo_font_face_destroy( _font );
	FT_Done_Face( _ft_face );
}

////////////////////////////////////////

}

