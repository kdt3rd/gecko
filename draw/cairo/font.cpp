
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
	cairo_font_options_set_subpixel_order( opts, CAIRO_SUBPIXEL_ORDER_DEFAULT );
	cairo_font_options_set_hint_style( opts, CAIRO_HINT_STYLE_FULL );
	cairo_font_options_set_hint_metrics( opts, CAIRO_HINT_METRICS_ON );

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

draw::font_extents font::extents( void )
{
	cairo_font_extents_t ex;
	cairo_scaled_font_extents( _font, &ex );
	return { ex.ascent, ex.descent, ex.height, ex.max_x_advance, ex.max_y_advance };
}

////////////////////////////////////////

draw::text_extents font::extents( const std::string &utf8 )
{
	cairo_text_extents_t ex;
	cairo_scaled_font_text_extents( _font, utf8.c_str(), &ex );
	return { ex.x_bearing, ex.y_bearing, ex.width, ex.height, ex.x_advance, ex.y_advance };
}

////////////////////////////////////////

}

