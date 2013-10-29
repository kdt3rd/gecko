
#include "font.h"
#include <stdexcept>
#include <cairo-ft.h>

namespace cairo
{

////////////////////////////////////////

font::font( cairo_font_face_t *face, std::string fam, std::string sty, double sz )
	: draw::font( std::move(fam), std::move(sty), sz ), _font( face )
{
}

////////////////////////////////////////

font::~font( void )
{
	cairo_font_face_destroy( _font );
}

////////////////////////////////////////

}

