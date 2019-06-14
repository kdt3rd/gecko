// Copyright (c) 2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "text.h"

#include <gl/element_buffer_data.h>
#include <gl/vertex_buffer_data.h>

namespace draw
{
////////////////////////////////////////

text::text( void ) : _scale( 1.F, 1.F ), _color( 0.f, 0.f, 0.f, 1.f ) {}

////////////////////////////////////////

text::text( const std::shared_ptr<script::font> &font )
    : _font( font ), _scale( 1.F, 1.F ), _color( 0.f, 0.f, 0.f, 1.f )
{}

////////////////////////////////////////

text::text( const std::string &utf8 )
    : _utf8( utf8 ), _scale( 1.F, 1.F ), _color( 0.f, 0.f, 0.f, 1.f )
{}

////////////////////////////////////////

text::text( const std::shared_ptr<script::font> &font, const std::string &utf8 )
    : _font( font )
    , _utf8( utf8 )
    , _scale( 1.F, 1.F )
    , _color( 0.f, 0.f, 0.f, 1.f )
{}

////////////////////////////////////////

void text::set_font( const std::shared_ptr<script::font> &font )
{
    _update = ( font != _font );
    _font   = font;
}

////////////////////////////////////////

void text::set_text( const std::string &utf8 )
{
    _update = ( utf8 != _utf8 );
    _utf8   = utf8;
}

////////////////////////////////////////

void text::set_scale( const point &s ) { _scale = s; }

////////////////////////////////////////

void text::set_position( const point &p ) { _pos = p; }

////////////////////////////////////////

void text::set_color( const color &c ) { _color = c; }

////////////////////////////////////////

void text::draw( platform::context &ctxt )
{
    if ( _update )
        update( ctxt );
    if ( _mesh.valid() )
    {
        auto &fgc = _stash->_font_glyph_cache;
        auto  x   = fgc.find( _font );
        if ( x != fgc.end() )
        {
            std::shared_ptr<gl::texture> texture = x->second.texture;

            gl::api &ogl = ctxt.api();
            ogl.save_matrix();
            ogl.model_matrix().scale(
                to_api( _scale[0] ), to_api( _scale[1] ) );
            ogl.model_matrix().translate(
                to_api( _pos[0] ), to_api( _pos[1] ) );

            auto txt = texture->bind();

            auto b = _mesh.bind();
            //			std::cout << "scale: " << _scale[0] << " (" << to_api( _scale[0] ) << ")"
            //					  << _scale[1] << " (" << to_api(_scale[1]) << ")" << std::endl;
            //			std::cout << "pos: " << _pos[0] << " (" << to_api( _pos[0] ) << ")"
            //		  			  << _pos[1] << " (" << to_api(_pos[1]) << ")" << std::endl;
            //			std::cout << "draw matrix: " << ogl.current_matrix() << std::endl;
            b.set_uniform( _mat_pos, ogl.current_matrix() );
            b.set_uniform( _col_pos, _color );
            b.set_uniform( _tex_pos, static_cast<int>( txt.unit() ) );
            b.draw();

            ogl.restore_matrix();
        }
    }
}

////////////////////////////////////////

void text::rebuild( platform::context & ) { _stash.reset(); }

////////////////////////////////////////

void text::update( platform::context &ctxt )
{
    _mesh.clear();
    if ( !_font || _utf8.empty() )
        return;

    _mesh = gl::mesh();
    // Create the geometry (with texture coordinates) for the text.
    gl::vertex_buffer_data<gl::vec2, gl::vec2> coords;
    gl::element_buffer_data                    tris;
    {
        using ptype    = script::font::coord_type;
        auto add_point = [&]( ptype cx, ptype cy, ptype tx, ptype ty ) {
            coords.push_back(
                { cx.count(), cy.count() }, { tx.count(), ty.count() } );
        };

        auto add_tri = [&]( uint32_t a, uint32_t b, uint32_t c ) {
            tris.push_back( a, b, c );
        };

        _font->render( add_point, add_tri, 0.0, 0.0, _utf8 );
    }

    // don't care about initialization, just need to have the structure
    if ( !_stash )
        ctxt.retrieve_common( this, _stash );

    // Update the font texture if needed.
    auto &fgc = _stash->_font_glyph_cache;
    auto  x   = fgc.find( _font );

    uint32_t curVer = _font->glyph_version();

    gl::api &                    ogl = ctxt.api();
    std::shared_ptr<gl::texture> texture;

    bool storeTex = true;
    if ( x != fgc.end() )
    {
        GlyphPack &pk = x->second;
        storeTex      = pk.version != curVer;
        texture       = pk.texture;
    }
    else
        texture = ogl.new_texture( gl::texture::target::IMAGE_2D );

    auto txt = texture->bind();

    if ( storeTex )
    {
        txt.set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
        txt.set_filters( gl::filter::LINEAR, gl::filter::LINEAR );
        txt.image_2d_red(
            gl::format::RED,
            static_cast<size_t>( _font->bitmap_width() ),
            static_cast<size_t>( _font->bitmap_height() ),
            gl::image_type::UNSIGNED_BYTE,
            _font->bitmap().data() );
        fgc[_font] = { _font->glyph_version(), texture };
    }

    auto prog = _stash->_program_cache;
    if ( !prog )
    {
        auto vshader = ogl.new_vertex_shader(
            R"SHADER(
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

        auto fshader = ogl.new_fragment_shader(
            R"SHADER(
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

        prog                   = ogl.new_program( vshader, fshader );
        _stash->_program_cache = prog;
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
    _update  = false;
}

////////////////////////////////////////

} // namespace draw
