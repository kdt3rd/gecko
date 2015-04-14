
#include "canvas.h"
#include "shaders.h"
#include <gl/check.h>

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

std::shared_ptr<gl::texture> canvas::gradient( const base::gradient &g, size_t n )
{
	std::vector<uint8_t> bytes( n * 4 );
	for ( size_t i = 0; i < n; ++i )
	{
		double stop = double(i)/double(n);
		base::color c = g.sample( stop );
		bytes[i*4+0] = clampColorByte( c.red() );
		bytes[i*4+1] = clampColorByte( c.green() );
		bytes[i*4+2] = clampColorByte( c.blue() );
		bytes[i*4+3] = clampColorByte( c.alpha() );
	}

	auto ret = new_texture();
	{
		auto txt = ret->bind( gl::texture::target::TEXTURE_RECTANGLE );
		txt.image_2d_rgba( gl::format::RGBA, bytes.size(), 1, gl::image_type::UNSIGNED_BYTE, bytes.data() );
	}
	return ret;
}

////////////////////////////////////////

std::shared_ptr<gl::program> canvas::program( const std::string &vert, const std::string &frag )
{
	auto vshader = new_shader( gl::shader::type::VERTEX, draw::shaders( vert ) );
	auto fshader = new_shader( gl::shader::type::FRAGMENT, draw::shaders( frag ) );
	return new_program( vshader, fshader );
}

////////////////////////////////////////

base::point
canvas::align_text( const std::shared_ptr<script::font> &font, const std::string &utf8, const base::rect &r, base::alignment a )
{
	base::rect rect;
	rect.set_x1( std::ceil( r.x1() ) );
	rect.set_y1( std::ceil( r.y1() ) );
	rect.set_x2( std::floor( r.x2() ) );
	rect.set_y2( std::floor( r.y2() ) );

	if ( ! font || utf8.empty() )
		return { 0.0, 0.0 };

	// TODO: add multi-line support?

	script::font_extents fex = font->extents();
	script::text_extents tex = font->extents( utf8 );

	double y = 0.0, x = 0.0;
	double textHeight = fex.ascent - fex.descent;

	switch ( a )
	{
		case base::alignment::CENTER:
		case base::alignment::LEFT:
		case base::alignment::RIGHT:
			y = rect.y() + std::round( ( rect.height() + textHeight ) / 2.0 ) + fex.descent;
			break;

		case base::alignment::BOTTOM:
		case base::alignment::BOTTOM_RIGHT:
		case base::alignment::BOTTOM_LEFT:
			y = rect.y2() - fex.descent;
			break;

		case base::alignment::TOP:
		case base::alignment::TOP_RIGHT:
		case base::alignment::TOP_LEFT:
			y = rect.y1() + fex.ascent;
			break;
	}

	switch ( a )
	{
		case base::alignment::LEFT:
		case base::alignment::TOP_LEFT:
		case base::alignment::BOTTOM_LEFT:
			x = rect.x() - tex.x_bearing;
			break;

		case base::alignment::RIGHT:
		case base::alignment::TOP_RIGHT:
		case base::alignment::BOTTOM_RIGHT:
			x = rect.x2() - tex.width - tex.x_bearing;
			break;

		case base::alignment::CENTER:
		case base::alignment::TOP:
		case base::alignment::BOTTOM:
			x = rect.x1() + std::round( ( rect.width() - tex.width - tex.x_bearing ) / 2.0 );
			break;
	}

	return { x, y };
}

////////////////////////////////////////

void
canvas::draw_text( const std::shared_ptr<script::font> &font, const base::point &p, const std::string &utf8, const base::paint &c )
{
	if ( ! font || utf8.empty() )
		return;

	font->render( _text_coord_buf, _text_texcoord_buf, _text_idx_buf, p, utf8 );

	auto x = _font_glyph_cache.find( font );
	uint32_t curVer = font->glyph_version();

	std::shared_ptr<gl::texture> texture;
	std::shared_ptr<gl::vertex_array> texture_vertices;

	bool storeTex = true;
	if ( x != _font_glyph_cache.end() )
	{
		GlyphPack &pk = x->second;
		storeTex = pk.version != curVer;
		texture = pk.texture;
	}
	else
		texture = new_texture();

	checkgl();
	auto txt = texture->bind( gl::texture::target::TEXTURE_2D );
	checkgl();

	if ( storeTex )
	{
		txt.set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	checkgl();
		txt.set_filters( gl::filter::LINEAR, gl::filter::LINEAR );
	checkgl();

		txt.image_2d_red( gl::format::RED,
					  font->bitmap_width(),
					  font->bitmap_height(),
					  gl::image_type::UNSIGNED_BYTE,
					  font->bitmap().data() );
	checkgl();

		_font_glyph_cache[font] = { font->glyph_version(), texture };
	}

	if ( ! _text_texture_vertices )
		_text_texture_vertices = new_buffer<float>();
	checkgl();
	if ( ! _text_output_vertices )
		_text_output_vertices = new_buffer<float>();
	checkgl();
	if ( ! _text_indices )
		_text_indices = new_buffer<uint16_t>();
	if ( ! _text_array )
		_text_array = new_vertex_array();
	checkgl();
	if ( ! _text_program )
		_text_program = program( "text.vert", "text_bitmap.frag" );
	checkgl();

	use_program( _text_program );
	checkgl();

	auto ta = _text_array->bind();
	checkgl();

	enable( gl::capability::BLEND );
	blend_func( gl::blend_style::SRC_ALPHA, gl::blend_style::ONE_MINUS_SRC_ALPHA );

	ta.attrib_pointer( _text_program->get_attribute_location( "text_tex_coords" ),
					   _text_texture_vertices, _text_texcoord_buf, 2 );
	ta.attrib_pointer( _text_program->get_attribute_location( "text_out_coords" ),
					   _text_output_vertices, _text_coord_buf, 2 );
	_text_program->set_uniform( "text_tex", GLint(0) );
	_text_program->set_uniform( "color", c.get_fill_color() );
	_text_program->set_uniform( "mvp_matrix", current_matrix() );

	auto idc = _text_indices->bind( gl::buffer<uint16_t>::target::ELEMENT_ARRAY_BUFFER );

	idc.data( _text_idx_buf, gl::usage::STATIC_DRAW );
	idc.draw( gl::primitive::TRIANGLES, _text_idx_buf.size() );
//	ta.draw_indices( gl::primitive::TRIANGLES, _text_idx_buf );
	// is this safe in GL ES? It's fine for normal OpenGL, and is fewer indices...
//	ta.draw_indices( gl::primitive::QUADS, _text_idx_buf );
	checkgl();

	disable( gl::capability::BLEND );
}

////////////////////////////////////////

/*
void canvas::fill( const base::color &c )
{
	paint p( c );
	fill( p );
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

