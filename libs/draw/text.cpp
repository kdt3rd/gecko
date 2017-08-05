//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "text.h"
#include <gl/vertex_buffer_data.h>
#include <gl/element_buffer_data.h>

namespace draw
{

std::weak_ptr<gl::program> text::_program_cache;
std::map<std::shared_ptr<script::font>, text::GlyphPack> text::_font_glyph_cache;

////////////////////////////////////////

text::text( void )
{
}

////////////////////////////////////////

text::text( const std::shared_ptr<script::font> &font )
	: _font( font )
{
}

////////////////////////////////////////

text::text( const std::shared_ptr<script::font> &font, const std::string &utf8 )
	: _font( font ), _utf8( utf8 )
{
}

////////////////////////////////////////

void text::set_font( const std::shared_ptr<script::font> &font )
{
	_update = ( font != _font );
	_font = font;
}

////////////////////////////////////////

void text::set_text( const std::string &utf8 )
{
	_update = ( utf8 != _utf8 );
	_utf8 = utf8;
}

////////////////////////////////////////

void text::set_position( const gl::vec2 &p )
{
	_pos = p;
}

////////////////////////////////////////

void text::set_color( const gl::color &c )
{
	_color = c;
}

////////////////////////////////////////

void text::draw( gl::api &ogl )
{
	if ( _update )
		update();
	if ( _mesh.valid() )
	{
		auto x = _font_glyph_cache.find( _font );
		if ( x != _font_glyph_cache.end() )
		{
			std::shared_ptr<gl::texture> texture = x->second.texture;

			gl::matrix4 local = gl::matrix4::translation( _pos[0], _pos[1] );

			auto txt = texture->bind();

			auto b = _mesh.bind();
			b.set_uniform( _mat_pos, local * ogl.current_matrix() );
			b.set_uniform( _col_pos, _color );
			b.set_uniform( _tex_pos, static_cast<int>( txt.unit() ) );
			b.draw();
		}
	}
}

////////////////////////////////////////

void text::update( void )
{
	_mesh.clear();
	if ( !_font || _utf8.empty() )
		return;

	_mesh = gl::mesh();
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

		_font->render( add_point, add_tri, base::point( 0.0, 0.0 ), _utf8 );
	}

	// Update the font texture if needed.
	auto x = _font_glyph_cache.find( _font );
	uint32_t curVer = _font->glyph_version();

	gl::api ogl;
	std::shared_ptr<gl::texture> texture;

	bool storeTex = true;
	if ( x != _font_glyph_cache.end() )
	{
		GlyphPack &pk = x->second;
		storeTex = pk.version != curVer;
		texture = pk.texture;
	}
	else
		texture = ogl.new_texture( gl::texture::target::IMAGE_2D );

	auto txt = texture->bind();

	if ( storeTex )
	{
		txt.set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
		txt.set_filters( gl::filter::LINEAR, gl::filter::LINEAR );
		txt.image_2d_red( gl::format::RED,
						  _font->bitmap_width(),
						  _font->bitmap_height(),
						  gl::image_type::UNSIGNED_BYTE,
						  _font->bitmap().data() );
		_font_glyph_cache[_font] = { _font->glyph_version(), texture };
	}

	auto prog = _program_cache.lock();
	if ( !prog )
	{
		auto vshader = ogl.new_vertex_shader( R"SHADER(
			#version 330

			layout(location = 0) in vec2 vertex_pos;
			layout(location = 1) in vec2 char_pos;

			uniform mat4 matrix;
			out vec2 tex_pos;

			void main()
			{
				tex_pos = char_pos;
				gl_Position = matrix * vec4( vertex_pos, 0.0, 1.0 );
			}
		)SHADER" );

		auto fshader = ogl.new_fragment_shader( R"SHADER(
			#version 330

			in vec2 tex_pos;
			out vec4 frag_color;

			uniform vec4 color;
			uniform sampler2D textTex;

			void main()
			{
				vec4 texColor = texture( textTex, tex_pos );
				frag_color = vec4( color.r, color.g, color.b, color.a * texColor.r );
			}
		)SHADER" );

		prog = ogl.new_program( vshader, fshader );
		_program_cache = prog;
	}

	_mesh.set_program( prog );

	{
		auto tbind = _mesh.bind();
		tbind.vertex_attribute( "vertex_pos", coords, 0 );
		tbind.vertex_attribute( "char_pos", coords, 1 );
		tbind.set_elements( tris );

		_mesh.add_triangles( tris.size() );
	}

	_mat_pos = _mesh.get_uniform_location( "matrix" );
	_col_pos = _mesh.get_uniform_location( "color" );
	_tex_pos = _mesh.get_uniform_location( "textTex" );
	_update = false;
}

////////////////////////////////////////

}

