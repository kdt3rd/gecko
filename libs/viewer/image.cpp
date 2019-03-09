// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "image.h"
#include "shaders.h"
#include <gl/opengl.h>
#include <media/image_frame.h>

namespace draw
{

////////////////////////////////////////

//image::image( void )
//{
//}
//
//////////////////////////////////////////
//
//image::image( const std::shared_ptr<gl::texture> &t )
//	: _texture( t )
//{
//}

////////////////////////////////////////

void image::set_color_state( const ::color::state &s )
{
	// TODO
}

////////////////////////////////////////

//void image::set_interleaved( int w, int h, gl::image_type it, void *ptr, int nchannels,
//							 int linestridebytes, base::endianness endian )
//{
//	clear_textures();
//#if 0
//	gl::api &ogl = context::current().hw_context().api();
//	_texture[0] = ogl.new_texture();
//	auto bind = _texture[0]->bind();
//	switch ( nchannels )
//	{
//		case 1:
//			bind.image_2d_red( gl::format::RED_FLOAT, w, h, it, ptr );
//			break;
//		case 2:
//			bind.image_2d_red( gl::format::RG_FLOAT, w, h, it, ptr );
//			break;
//		case 3:
//			bind.image_2d_red( gl::format::RGB_FLOAT, w, h, it, ptr );
//			break;
//		case 4:
//			bind.image_2d_red( gl::format::RGBA_FLOAT, w, h, it, ptr );
//			break;
//		default:
//			break;
//	}
//#endif
//}
//
//////////////////////////////////////////
//
//void image::set_planar( int w, int h,
//						gl::image_type rit, void *red,
//						gl::image_type git, void *green,
//						gl::image_type bit, void *blue,
//						gl::image_type ait, void *alpha )
//{
//	clear_textures();
//#if 0
//	gl::api &ogl = context::current().hw_context().api();
//	if ( red )
//	{
//		_texture[0] = ogl.new_texture();
//		auto bind = _texture[0]->bind();
//		bind.image_2d_red( gl::format::RED_FLOAT, w, h, rit, red );
//	}
//	if ( green )
//	{
//		precondition( red, "expect red pointer if providing green" );
//		_texture[1] = ogl.new_texture();
//		auto bind = _texture[1]->bind();
//		bind.image_2d_red( gl::format::RED_FLOAT, w, h, git, green );
//	}
//	if ( blue )
//	{
//		precondition( red && green, "expect red & green pointer if providing blue" );
//		_texture[2] = ogl.new_texture();
//		auto bind = _texture[2]->bind();
//		bind.image_2d_red( gl::format::RED_FLOAT, w, h, bit, blue );
//	}
//	if ( alpha )
//	{
//		_texture[3] = ogl.new_texture();
//		auto bind = _texture[3]->bind();
//		bind.image_2d_red( gl::format::RED_FLOAT, w, h, ait, alpha );
//	}
//#endif
//}

////////////////////////////////////////

void image::convert( gl::api &ogl, const media::frame &fr )
{
	throw_not_yet();
#if 0
	// TODO: enable interleaved frame
	size_t nChannels = fr.size();
	_dx = fr.x1();
	_dy = fr.y1();

	int w = static_cast<int>( fr.width() );
	int h = static_cast<int>( fr.height() );

	_w = static_cast<float>( w );
	_h = static_cast<float>( h );

	_interleaved = fr.is_interleaved();
	// never true for 1 channel images...
	if ( _interleaved )
	{
		precondition( nChannels <= 4, "expect max of 4 channels for interleaved, got {0}", nChannels );

		if ( ! _texture[0] )
			_texture[0] = ogl.new_texture( gl::texture::target::RECTANGLE );
		_texture[1].reset();
		_texture[2].reset();
		_texture[3].reset();

		auto bind = _texture[0]->bind();
		gl::image_type it = gl::image_type::UNSIGNED_BYTE;
		const media::image_buffer &b = fr[0];
		switch ( b.bits() )
		{
			case 8:
				break;
			case 10:
			case 12:
			case 14:
			case 16:
				if ( b.is_floating() )
					it = gl::image_type::HALF;
				else
					it = gl::image_type::UNSIGNED_SHORT;
				break;
			case 32:
				if ( b.is_floating() )
					it = gl::image_type::FLOAT;
				else
					it = gl::image_type::UNSIGNED_INT;
				break;
			default:
				throw_runtime( "unsupported bit formation" );
		}

		const void *p = b.data();
		size_t stride = static_cast<size_t>( b.ystride_bytes() );
		if ( nChannels == 2 )
		{
			bind.image_2d_rg( gl::format::RG_FLOAT, static_cast<size_t>( w ),
			                  static_cast<size_t>( h ), it, p, stride,
							  b.endianness() != base::endianness::NATIVE );
		}
		else if ( nChannels == 3 )
		{
			bind.image_2d_rgb( gl::format::RGB_FLOAT, static_cast<size_t>( w ),
			                   static_cast<size_t>( h ), it, p, stride,
							   b.endianness() != base::endianness::NATIVE );
		}
		else if ( nChannels == 4 )
		{
			bind.image_2d_rgba( gl::format::RGBA_FLOAT, static_cast<size_t>( w ),
			                    static_cast<size_t>( h ), it, p, stride,
								b.endianness() != base::endianness::NATIVE );
		}
		if ( _stash )
			_mesh.set_program( _stash->_interleave_prog );
	}
	else
	{
		for ( size_t c = 0; c < std::min( size_t(4), nChannels ); ++c )
		{
			if ( ! _texture[c] )
				_texture[c] = ogl.new_texture( gl::texture::target::RECTANGLE );
			auto bind = _texture[c]->bind();
			const media::image_buffer &b = fr[c];
			gl::image_type it = gl::image_type::UNSIGNED_BYTE;
			switch ( b.bits() )
			{
				case 8:
					break;
				case 10:
				case 12:
				case 14:
				case 16:
					if ( b.is_floating() )
						it = gl::image_type::HALF;
					else
						it = gl::image_type::UNSIGNED_SHORT;
					break;
				case 32:
					if ( b.is_floating() )
						it = gl::image_type::FLOAT;
					else
						it = gl::image_type::UNSIGNED_INT;
					break;
				default:
					throw_runtime( "unsupported bit formation" );
			}
			const uint8_t *p = static_cast<const uint8_t *>( b.data() );
			p += b.offset() / 8;
			size_t stride = static_cast<size_t>( b.ystride_bytes() );
			bind.image_2d_red( gl::format::RED_FLOAT, static_cast<size_t>( w ),
			                   static_cast<size_t>( h ), it, p, stride,
							   b.endianness() != base::endianness::NATIVE );
		}
		if ( _stash )
			_mesh.set_program( _stash->_planar_prog );
	}
#endif
}

////////////////////////////////////////

void image::clear( void )
{
	_w = _h = 0;
	_texture[0].reset();
	_texture[1].reset();
	_texture[2].reset();
	_texture[3].reset();
}

////////////////////////////////////////

void image::set_pan( float x, float y )
{
	_rect.translate( x, y );
}

////////////////////////////////////////

void image::add_zoom( float pivx, float pivy, float zoom )
{
	// TODO: finish adding pivot once we have that
	_rect.scale( zoom, zoom );
}

////////////////////////////////////////

void image::set_filtering( zoom_filter f )
{
	_filter = f;
}

////////////////////////////////////////

int image::num_textures( void ) const
{
	int cnt = 0;
	for ( int i = 0; i < 4; ++i )
		cnt += (_texture[i]) ? 1 : 0;
	return cnt;
}

////////////////////////////////////////

void image::set_texture_offset( int offset )
{
	_texture_offset = offset;
}

////////////////////////////////////////

void image::reset_position( int parw, int parh )
{
	int cenx = parw / 2;
	int ceny = parh / 2;
	_rect = gl::matrix4();

	int imgcenx = _w / 2;
	int imgceny = _h / 2;

	set_pan( float( cenx - imgcenx ), float( ceny - imgceny ) );
}

void image::rebuild( platform::context &ctxt )
{
	_stash.reset();
}

////////////////////////////////////////

void image::draw( platform::context &ctxt )
{
	initialize( ctxt );
	gl::filter gf = ( _filter == zoom_filter::nearest ) ? gl::filter::NEAREST : gl::filter::LINEAR;

	if ( _interleaved )
	{
		if ( _texture[0] )
		{
			auto bt = _texture[0]->bind( static_cast<size_t>( _texture_offset ) );
			bt.set_filters( gf, gf );
			auto bound = _mesh.bind();
			//gl::matrix4 tmp = _rect;
			//tmp.scale( 0.5, 0.5 );
			//tmp.translate( -0.5, -0.5 );
			//bound.set_uniform( _stash->_matrix_loc, tmp );
			bound.set_uniform( _stash->_i_matrix_loc, _rect * ctxt.api().current_matrix() );
			bound.set_uniform( _stash->_i_tex_unit_loc, _texture_offset );
			bound.draw();
		}
	}
	else
	{
		if ( _texture[0] && _texture[1] && _texture[2] && _texture[3] )
		{
			auto bt0 = _texture[0]->bind( static_cast<size_t>( _texture_offset ) );
			bt0.set_filters( gf, gf );

			auto bt1 = _texture[1]->bind( static_cast<size_t>( _texture_offset + 1 ) );
			bt1.set_filters( gf, gf );

			auto bt2 = _texture[2]->bind( static_cast<size_t>( _texture_offset + 2 ) );
			bt2.set_filters( gf, gf );

			auto bt3 = _texture[3]->bind( static_cast<size_t>( _texture_offset + 3 ) );
			bt3.set_filters( gf, gf );

			auto bound = _mesh.bind();
			bound.set_uniform( _stash->_p_matrix_loc, _rect * ctxt.api().current_matrix() );
			bound.set_uniform( _stash->_p_tex_unit0_loc, _texture_offset );
			bound.set_uniform( _stash->_p_tex_unit1_loc, _texture_offset + 1 );
			bound.set_uniform( _stash->_p_tex_unit2_loc, _texture_offset + 2 );
			bound.set_uniform( _stash->_p_tex_unit3_loc, _texture_offset + 3 );
			bound.set_uniform( _stash->_p_num_chans, GLint(4) );
			bound.draw();
		}
		else if ( _texture[0] && _texture[1] && _texture[2] )
		{
			auto bt0 = _texture[0]->bind( static_cast<size_t>( _texture_offset ) );
			bt0.set_filters( gf, gf );

			auto bt1 = _texture[1]->bind( static_cast<size_t>( _texture_offset + 1 ) );
			bt1.set_filters( gf, gf );

			auto bt2 = _texture[2]->bind( static_cast<size_t>( _texture_offset + 2 ) );
			bt2.set_filters( gf, gf );

			auto bound = _mesh.bind();
			bound.set_uniform( _stash->_p_matrix_loc, _rect * ctxt.api().current_matrix() );
			bound.set_uniform( _stash->_p_tex_unit0_loc, _texture_offset );
			bound.set_uniform( _stash->_p_tex_unit1_loc, _texture_offset + 1 );
			bound.set_uniform( _stash->_p_tex_unit2_loc, _texture_offset + 2 );
			bound.set_uniform( _stash->_p_num_chans, GLint(3) );
			bound.draw();
		}
		else if ( _texture[0] && _texture[1] )
		{
			auto bt0 = _texture[0]->bind( static_cast<size_t>( _texture_offset ) );
			bt0.set_filters( gf, gf );

			auto bt1 = _texture[1]->bind( static_cast<size_t>( _texture_offset + 1 ) );
			bt1.set_filters( gf, gf );

			auto bound = _mesh.bind();
			bound.set_uniform( _stash->_p_matrix_loc, _rect * ctxt.api().current_matrix() );
			bound.set_uniform( _stash->_p_tex_unit0_loc, _texture_offset );
			bound.set_uniform( _stash->_p_tex_unit1_loc, _texture_offset + 1 );
			bound.set_uniform( _stash->_p_num_chans, GLint(2) );
			bound.draw();
		}
		else if ( _texture[0] )
		{
			auto bt0 = _texture[0]->bind( static_cast<size_t>( _texture_offset ) );
			bt0.set_filters( gf, gf );

			auto bound = _mesh.bind();
			bound.set_uniform( _stash->_p_matrix_loc, _rect * ctxt.api().current_matrix() );
			bound.set_uniform( _stash->_p_tex_unit0_loc, _texture_offset );
			bound.set_uniform( _stash->_p_num_chans, GLint(1) );
			bound.draw();
		}
	}
}

////////////////////////////////////////

void image::initialize( platform::context &ctxt )
{
	gl::api &ogl = ctxt.api();
	if ( ! _stash )
	{
		if ( ctxt.retrieve_common( this, _stash ) )
		{
			try
			{
				_stash->_interleave_prog = ogl.new_program();
				_stash->_interleave_prog->attach( ogl.new_vertex_shader( viewer_shaders( "drawImage.vert" ) ) );
//			_stash->_prog->attach( ogl.new_fragment_shader( viewer_shaders( "convertDisplay.frag" ) ) );
				_stash->_interleave_prog->attach( ogl.new_fragment_shader( viewer_shaders( "drawImage.frag" ) ) );

				_stash->_interleave_prog->link();

				_stash->_planar_prog = ogl.new_program();
				_stash->_planar_prog->attach( ogl.new_vertex_shader( viewer_shaders( "drawImage.vert" ) ) );
//			_stash->_prog->attach( ogl.new_fragment_shader( viewer_shaders( "convertDisplay.frag" ) ) );
				_stash->_planar_prog->attach( ogl.new_fragment_shader( viewer_shaders( "drawImagePlanar.frag" ) ) );

				_stash->_planar_prog->link();
			}
			catch ( ... )
			{
				throw_add_location( "creating image drawing shaders" );
			}


			_stash->_i_tex_unit_loc = _stash->_interleave_prog->get_uniform_location( "tex_unit" );
			_stash->_i_matrix_loc = _stash->_interleave_prog->get_uniform_location( "matrix" );

			_stash->_p_num_chans = _stash->_planar_prog->get_uniform_location( "num_chans" );
			_stash->_p_tex_unit0_loc = _stash->_planar_prog->get_uniform_location( "tex_unitR" );
			_stash->_p_tex_unit1_loc = _stash->_planar_prog->get_uniform_location( "tex_unitG" );
			_stash->_p_tex_unit2_loc = _stash->_planar_prog->get_uniform_location( "tex_unitB" );
			_stash->_p_tex_unit3_loc = _stash->_planar_prog->get_uniform_location( "tex_unitA" );
			_stash->_p_matrix_loc = _stash->_planar_prog->get_uniform_location( "matrix" );
		}

		if ( _interleaved )
			_mesh.set_program( _stash->_interleave_prog );
		else
			_mesh.set_program( _stash->_planar_prog );

		// Setup vertices
		gl::vertex_buffer_data<gl::vec2> vertices;
		vertices.push_back( { 0.F, 0.F } );
		vertices.push_back( { _w, 0.F } );
		vertices.push_back( { _w, _h } );
		vertices.push_back( { 0.F, _h } );
		vertices.vbo( gl::buffer_usage::STATIC_DRAW );

		gl::element_buffer_data elements { 0, 3, 1, 1, 3, 2 };
		{
			auto bound = _mesh.bind();
			bound.vertex_attribute( "position", vertices );
			bound.set_elements( elements );
		}

		_mesh.add_triangles( 6 );
	}
}

#if 0
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
#endif

////////////////////////////////////////

void image::clear_textures( void )
{
	for ( int i = 0; i < 4; ++i )
		_texture[i].reset();
}

////////////////////////////////////////

}
