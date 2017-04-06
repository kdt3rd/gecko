//
// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "viewer.h"
#include <viewer/shaders.h>
#include <draw/object.h>
//#include <draw/image.h>
#include <gl/png_image.h>

////////////////////////////////////////

namespace
{
	float scaling( int z )
	{
		if ( z <= 0 )
			return -1.F / float( z - 2 );
		return z;
	}
}

////////////////////////////////////////

namespace viewer
{

////////////////////////////////////////

viewer::viewer( void )
{
	// seem to get GL errors / crashes if we do this here as the context
	// is around but the window hasn't yet been shown or something
	// this is debug anyway...
//	set_texture_a( std::make_shared<gl::texture>( gl::png_read( "/home/kimball/Images/mandril.png" ) ) );
//	set_texture_b( std::make_shared<gl::texture>( gl::png_read( "/home/kimball/Images/lena.png" ) ) );

	_zoomA = 1;
	_panB.set( 512, 0 );
}

////////////////////////////////////////

void viewer::paint( const std::shared_ptr<draw::canvas> &c )
{
	if ( !_prog )
	{
		try
		{
			_prog = c->new_program();
			_prog->attach( c->new_shader( gl::shader::type::VERTEX, ::viewer::shaders( "shader.vert" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "shader.frag" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "textureA.frag" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "textureB.frag" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "nolutA.frag" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "nolutB.frag" ) ) );
			_prog->attach( c->new_shader( gl::shader::type::FRAGMENT, ::viewer::shaders( "nowipe.frag" ) ) );
			_prog->link();
		}
		catch ( ... )
		{
			throw_add_location( "creating image drawing shaders" );
		}
	}
	c->use_program( _prog );

	if ( !_quad )
		_quad = c->new_vertex_array();

	// coordinates
	std::vector<float> vertices =
	{
		0, 0,
		float(width()), 0,
		0, float(height()),
		float(width()), float(height())
	};

	auto buf = c->new_array_buffer<float>( vertices );
	{
		auto va = _quad->bind();
		va.attrib_pointer( _prog->get_attribute_location( "position" ), buf, 2, 2, 0 );
	}

	if ( _textureA )
		_textureA->bind( gl::texture::target::TEXTURE_RECTANGLE, 0 );

	if ( _textureB )
		_textureB->bind( gl::texture::target::TEXTURE_RECTANGLE, 1 );

	float zA = scaling( _zoomA );
	float zB = scaling( _zoomB );
	auto matA = gl::matrix4::scaling( zA, zA ) * gl::matrix4::translation( _panA );
	auto matB = gl::matrix4::scaling( zB, zB ) * gl::matrix4::translation( _panB ) * matA;

	_prog->set_uniform( "mvpMatrix", c->current_matrix() );
	_prog->set_uniform( "winW", width() );
	_prog->set_uniform( "winH", width() );
	_prog->set_uniform( "wipeX", 0.5 );
	_prog->set_uniform( "wipeY", 0.5 );
	_prog->set_uniform( "texMatA", matA.inverted() );
	_prog->set_uniform( "texMatB", matB.inverted() );
	_prog->set_uniform( "texA", 0 );
	_prog->set_uniform( "texB", 1 );

	auto va = _quad->bind();
	va.draw( gl::primitive::TRIANGLE_STRIP, 0, 4 );
}

////////////////////////////////////////

void viewer::set_texture_a( const std::shared_ptr<gl::texture> &t )
{
	_textureA = t;
	{
		auto txt = _textureA->bind( gl::texture::target::TEXTURE_RECTANGLE, 0 );
		txt.set_wrapping( gl::wrapping::CLAMP_TO_BORDER );
		txt.set_border_color( base::color( 0, 0, 0, 0 ) );
	}
}

////////////////////////////////////////

void viewer::set_texture_b( const std::shared_ptr<gl::texture> &t )
{
	_textureB = t;
	{
		auto txt = _textureB->bind( gl::texture::target::TEXTURE_RECTANGLE, 0 );
		txt.set_wrapping( gl::wrapping::CLAMP_TO_BORDER );
		txt.set_border_color( base::color( 0, 0, 0, 0 ) );
	}
}

////////////////////////////////////////

void viewer::compute_minimum( void )
{
	widget::compute_minimum();
}

////////////////////////////////////////

bool viewer::mouse_press( const base::point &p, int button )
{
	if ( button == 1 )
	{
		_panningA = true;
		_last = p;
		return true;
	}
	else if ( button == 2 )
	{
		_panningB = true;
		_last = p;
		return true;
	}
	return widget::mouse_press( p, button );
}

////////////////////////////////////////

bool viewer::mouse_move( const base::point &p )
{
	if ( _panningA )
	{
		base::point delta = ( p - _last );
		_panA = _panA + delta;
		_last = p;
		invalidate();
	}
	else if ( _panningB )
	{
		float zA = scaling( _zoomA );
		base::point delta = ( p - _last );
		_panB = _panB + delta * ( 1.F / zA );
		_last = p;
		invalidate();
	}
	return widget::mouse_move( p );
}

////////////////////////////////////////

bool viewer::mouse_release( const base::point & /*p*/, int button )
{
	if ( button == 1 )
	{
		_panningA = false;
		return true;
	}
	if ( button == 2 )
	{
		_panningB = false;
		return true;
	}
	return false;
}

////////////////////////////////////////

bool viewer::mouse_wheel( int amount )
{
	_zoomB += amount;

	invalidate();
	return true;
}

////////////////////////////////////////

}

