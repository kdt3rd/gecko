
#include "canvas.h"

////////////////////////////////////////

namespace
{
	inline uint8_t clampColorByte( double c )
	{
		c = std::max( 0.0, std::min( 255.0, std::floor( c * 256.0 ) ) );
		return uint8_t( c );
	}
}

////////////////////////////////////////

namespace draw
{

////////////////////////////////////////

canvas::canvas( void )
{
}

////////////////////////////////////////

canvas::~canvas( void )
{
}

////////////////////////////////////////

std::shared_ptr<gl::texture> canvas::gradient( core::gradient &g, size_t n )
{
	std::vector<uint8_t> bytes( n * 4 );
	for ( size_t i = 0; i < n; ++i )
	{
		double stop = double(i)/double(n);
		core::color c = g.sample( stop );
		bytes[i*4+0] = clampColorByte( c.red() );
		bytes[i*4+1] = clampColorByte( c.green() );
		bytes[i*4+2] = clampColorByte( c.blue() );
		bytes[i*4+3] = clampColorByte( c.alpha() );
	}

	auto ret = new_texture();
	{
		auto txt = ret->bind( gl::texture::target::TEXTURE_RECTANGLE );
		txt.image_2d( gl::format::RGBA, bytes.size(), 1, gl::image_type::UNSIGNED_BYTE, bytes.data() );
	}
	return ret;
}

////////////////////////////////////////

/*
void canvas::fill( const core::color &c )
{
	paint p( c );
	fill( p );
}

////////////////////////////////////////

core::point canvas::align_text( const std::shared_ptr<font> &font, const std::string &text, const core::rect &rect, alignment a )
{
	font_extents fex = font->extents();
	text_extents tex = font->extents( text );

	double y = 0.0, x = 0.0;

	switch ( a )
	{
		case alignment::CENTER:
		case alignment::LEFT:
		case alignment::RIGHT:
			y = rect.y() + ( rect.height() + fex.ascent ) / 2.0;
			break;

		case alignment::BOTTOM:
		case alignment::BOTTOM_RIGHT:
		case alignment::BOTTOM_LEFT:
			y = rect.y2() - fex.descent;
			break;

		case alignment::TOP:
		case alignment::TOP_RIGHT:
		case alignment::TOP_LEFT:
			y = rect.y1() + fex.ascent;
			break;
	}

	switch ( a )
	{
		case alignment::LEFT:
		case alignment::TOP_LEFT:
		case alignment::BOTTOM_LEFT:
			x = rect.x() - tex.x_bearing;
			break;

		case alignment::RIGHT:
		case alignment::TOP_RIGHT:
		case alignment::BOTTOM_RIGHT:
			x = rect.x2() - tex.width;
			break;

		case alignment::CENTER:
		case alignment::TOP:
		case alignment::BOTTOM:
			x = rect.x1() + ( rect.width() - tex.width ) /2.0;
			break;
	}

	return { x, y };
}

////////////////////////////////////////

void canvas::present( void )
{
}
*/

////////////////////////////////////////

void canvas::save( void )
{
	save_matrix();
}

////////////////////////////////////////

void canvas::restore( void )
{
	restore_matrix();
}

////////////////////////////////////////

}

