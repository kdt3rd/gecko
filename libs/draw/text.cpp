
#include "text.h"
#include <gl/vertex_buffer_data.h>
#include <gl/element_buffer_data.h>

namespace draw
{

std::map<std::shared_ptr<script::font>, text::GlyphPack> text::_font_glyph_cache;

////////////////////////////////////////

text::text( const std::shared_ptr<script::font> &font )
	: _font( font )
{
}

////////////////////////////////////////

void text::set_text( const std::string &utf8 )
{
	_update = true;
	_utf8 = utf8;
}

////////////////////////////////////////

void text::set_position( const gl::vec2 &p )
{
	_update = true;
	_pos = p;
}

////////////////////////////////////////

void text::set_color( const gl::color &c )
{
	_update = true;
	_color = c;
}

////////////////////////////////////////

void text::render( void )
{
	if ( _update )
		update();
	if ( _mesh.valid() )
	{
		auto b = _mesh.bind();
		b.draw();
	}
}

////////////////////////////////////////

void text::update( void )
{
	_mesh.clear();
	if ( !_font || _utf8.empty() )
		return;

	// Create the geometry (with texture coordinates) for the text.
	gl::vertex_buffer_data<gl::vec2,gl::vec2> coords;
	gl::element_buffer_data tris;
	{
		auto add_point = [&]( float cx, float cy, float tx, float ty )
		{
			coords.push_back( { cx, cy }, { tx, ty } );
		};

		auto add_tri = [&]( size_t a, size_t b, size_t c )
		{
			tris.push_back( a, b, c );
		};

		_font->render( add_point, add_tri, base::point( _pos[0], _pos[1] ), _utf8 );
	}

	// Update the font texture if needed.
	auto x = _font_glyph_cache.find( _font );
	uint32_t curVer = _font->glyph_version();

	std::shared_ptr<gl::texture> texture;

	bool storeTex = true;
	if ( x != _font_glyph_cache.end() )
	{
		GlyphPack &pk = x->second;
		storeTex = pk.version != curVer;
		texture = pk.texture;
	}
	else
	{
		gl::api ogl;
		texture = ogl.new_texture();
	}

	/*
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
	*/
}

////////////////////////////////////////

}

