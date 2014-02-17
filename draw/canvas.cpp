
#include "canvas.h"
#include <draw/shaders.h>

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

std::shared_ptr<gl::texture> canvas::gradient( const core::gradient &g, size_t n )
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

std::shared_ptr<gl::program> canvas::program( const std::string &vert, const std::string &frag )
{
	auto vshader = new_shader( gl::shader::type::VERTEX, draw::shaders( vert ) );
	auto fshader = new_shader( gl::shader::type::FRAGMENT, draw::shaders( frag ) );
	return new_program( vshader, fshader );
}

////////////////////////////////////////

core::point
canvas::align_text( const std::shared_ptr<font> &font, const std::string &utf8, const core::rect &rect, alignment a )
{
	if ( ! font || utf8.empty() )
		return { 0.0, 0.0 };

	// TODO: add multi-line support?

	font_extents fex = font->extents();
	text_extents tex = font->extents( utf8 );

	double y = 0.0, x = 0.0;

	switch ( a )
	{
		case alignment::CENTER:
		case alignment::LEFT:
		case alignment::RIGHT:
			y = rect.y() + rect.height() / 2.0 - fex.descent;
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
			x = rect.x2() - tex.width - tex.x_bearing;
			break;

		case alignment::CENTER:
		case alignment::TOP:
		case alignment::BOTTOM:
			x = rect.x1() + ( rect.width() - tex.width - tex.x_bearing ) /2.0;
			break;
	}

	return { x, y };
}

////////////////////////////////////////

void
canvas::draw_text( const std::shared_ptr<font> &font, const core::point &p, const std::string &utf8, const core::paint &c )
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

	auto txt = texture->bind( gl::texture::target::TEXTURE_2D );

	if ( storeTex )
	{
		txt.set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
		txt.set_filters( gl::filter::LINEAR, gl::filter::LINEAR );

		txt.image_2d( gl::format::RED,
					  font->glyph_texture_size_x(),
					  font->glyph_texture_size_y(),
					  gl::image_type::UNSIGNED_BYTE,
					  font->glyph_texture().data() );

		_font_glyph_cache[font] = { font->glyph_version(), texture };
	}

	if ( ! _text_texture_vertices )
		_text_texture_vertices = new_buffer<float>();
	if ( ! _text_output_vertices )
		_text_output_vertices = new_buffer<float>();
//	if ( ! _text_indices )
//		_text_indices = new_buffer<uint16_t>();
	if ( ! _text_array )
		_text_array = new_vertex_array();
	if ( ! _text_program )
		_text_program = program( "text.vert", "text_bitmap.frag" );

	use_program( _text_program );

	auto ta = _text_array->bind();

	enable( gl::capability::BLEND );
	blend_func( gl::blend_style::SRC_ALPHA, gl::blend_style::ONE_MINUS_SRC_ALPHA );

	ta.attrib_pointer( _text_program->get_attribute_location( "text_tex_coords" ),
					   _text_texture_vertices, _text_texcoord_buf, 2 );
	ta.attrib_pointer( _text_program->get_attribute_location( "text_out_coords" ),
					   _text_output_vertices, _text_coord_buf, 2 );
	_text_program->set_uniform( "text_tex", GLint(0) );
	_text_program->set_uniform( "color", c.get_fill_color() );
	_text_program->set_uniform( "mvp_matrix", current_matrix() );

//	auto idc = _text_indices->bind( gl::buffer<uint16_t>::target::ELEMENT_ARRAY_BUFFER );

//	idc.data( _text_idx_buf, gl::usage::STATIC_DRAW );
//	idc.draw( gl::primitive::TRIANGLES, _text_idx_buf.size() );
//	ta.draw_indices( gl::primitive::TRIANGLES, _text_idx_buf );
	// is this safe in GL ES? It's fine for normal OpenGL, and is fewer indices...
	ta.draw_indices( gl::primitive::QUADS, _text_idx_buf );

	disable( gl::capability::BLEND );
}

////////////////////////////////////////

/*
void canvas::fill( const core::color &c )
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

