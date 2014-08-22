
#include "image.h"
#include "shaders.h"
#include <gl/opengl.h>

namespace draw
{

////////////////////////////////////////

image::image( void )
{
}

////////////////////////////////////////

image::image( const std::shared_ptr<gl::texture> &t )
	: _texture( t )
{
}

////////////////////////////////////////

void image::draw( gl::context &ctxt )
{
	// Create everything
	if ( !_texture )
		return;

	if ( !_prog )
	{
		try
		{
			auto vshader = ctxt.new_shader( gl::shader::type::VERTEX, draw::shaders( "position_uv.vert" ) );
			auto fshader = ctxt.new_shader( gl::shader::type::FRAGMENT, draw::shaders( "texture.frag" ) );
			_prog = ctxt.new_program( vshader, fshader );
		}
		catch ( ... )
		{
			throw_add_location( "creating image drawing shaders" );
		}
	}

	if ( !_quad )
	{
		_quad = ctxt.new_vertex_array();

		// coordinates
		std::vector<float> vertices =
		{
			0, 0,
			_w, 0,
			0, _h,
			_w, _h
		};

		auto buf = ctxt.new_buffer<float>( gl::buffer<float>::target::ARRAY_BUFFER, vertices, gl::usage::STREAM_DRAW );
		auto va = _quad->bind();
		va.attrib_pointer( _prog->get_attribute_location( "position" ), buf, 2, 2, 0 );
	}

	_texture->bind( gl::texture::target::TEXTURE_RECTANGLE, 1 );
	_prog->set_uniform( "mvp_matrix", ctxt.current_matrix() );
	_prog->set_uniform( "txt", 1 );
	ctxt.use_program( _prog );

	auto va = _quad->bind();
	va.draw( gl::primitive::TRIANGLE_STRIP, 0, 4 );
}

////////////////////////////////////////

}

