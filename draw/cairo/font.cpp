
#include "font.h"
#include <stdexcept>
#include <cairo-ft.h>

namespace cairo
{

////////////////////////////////////////

font::font( cairo_font_face_t *face, std::string fam, std::string sty, double sz )
	: draw::font( std::move(fam), std::move(sty), sz )
{
	auto opts = cairo_font_options_create();
	cairo_font_options_set_antialias( opts, CAIRO_ANTIALIAS_SUBPIXEL );

	cairo_matrix_t mat, ctm;
	cairo_matrix_init_identity( &ctm );
	cairo_matrix_init_scale( &mat, sz, sz );

	_font = cairo_scaled_font_create( face, &mat, &ctm, opts );

	cairo_font_options_destroy( opts );
}

////////////////////////////////////////

font::~font( void )
{
	cairo_scaled_font_destroy( _font );
}

////////////////////////////////////////

}

