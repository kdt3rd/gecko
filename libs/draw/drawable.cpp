//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "drawable.h"
#include "shaders.h"
#include <gl/api.h>

namespace draw
{

std::map<std::string,std::weak_ptr<gl::program>> drawable::_program_cache;

////////////////////////////////////////

drawable::~drawable( void )
{
}

////////////////////////////////////////

std::shared_ptr<gl::texture> drawable::new_gradient( gl::api &ogl, const draw::gradient &g, size_t n )
{
	std::vector<uint8_t> bytes( n * 4 );
	for ( size_t i = 0; i < n; ++i )
	{
		double stop = double(i)/double(n-1);
		gl::color c = g.sample( stop );
		bytes[i*4+0] = static_cast<uint8_t>( std::max( 0.F, std::min( 255.F, std::floor( c.red() * 256.F ) ) ) );
		bytes[i*4+1] = static_cast<uint8_t>( std::max( 0.F, std::min( 255.F, std::floor( c.green() * 256.F ) ) ) );
		bytes[i*4+2] = static_cast<uint8_t>( std::max( 0.F, std::min( 255.F, std::floor( c.blue() * 256.F ) ) ) );
		bytes[i*4+3] = static_cast<uint8_t>( std::max( 0.F, std::min( 255.F, std::floor( c.alpha() * 256.F ) ) ) );
	}

	auto ret = ogl.new_texture( gl::texture::target::RECTANGLE );
	{
		auto txt = ret->bind();
		txt.image_2d_rgba( gl::format::RGBA, n, 1, gl::image_type::UNSIGNED_BYTE, bytes.data() );
	}
	return ret;
}

////////////////////////////////////////

std::shared_ptr<gl::program> drawable::new_program( gl::api &ogl, const std::string &vert, const std::string &frag, bool cached )
{
	std::shared_ptr<gl::program> result;
	std::string lookup = vert + '|' + frag;

	if ( cached )
	{
		auto prog = _program_cache.find( lookup );
		if ( prog != _program_cache.end() )
			result = prog->second.lock();
	}

	if ( !result )
	{
		result = std::make_shared<gl::program>(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( vert ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( frag ) )
		);
		if ( cached )
			_program_cache[lookup] = result;
	}

	return result;
}

////////////////////////////////////////

std::shared_ptr<gl::texture> drawable::get_fill_texture( gl::api &ogl, const paint &p )
{
	std::shared_ptr<gl::texture> result;
	if ( p.has_fill_color() )
	{
	}
	else if ( p.has_fill_linear() )
	{
		result = new_gradient( ogl, p.get_fill_linear_gradient() );
		result->bind().set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	}
	else if ( p.has_fill_radial() )
	{
		result = new_gradient( ogl, p.get_fill_radial_gradient() );
		result->bind().set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	}
	else if ( p.has_fill_conical() )
	{
		result = new_gradient( ogl, p.get_fill_conical_gradient() );
		result->bind().set_wrapping( gl::wrapping::REPEAT );
	}
	else if ( p.has_fill_box() )
	{
		result = new_gradient( ogl, p.get_fill_box_gradient() );
		result->bind().set_wrapping( gl::wrapping::CLAMP_TO_EDGE );
	}
	else if ( p.has_no_fill() )
	{
	}
	else
		throw std::runtime_error( "unhandled fill type" );

	return result;
}

////////////////////////////////////////

gl::program::uniform drawable::fill_mesh( gl::api &ogl, gl::mesh &m, const paint &p, const std::string &vert )
{
	if ( p.has_fill_color() )
	{
		m.set_program( new_program( ogl, vert, "single_color.frag", false ) );
		m.get_program().use();
		m.set_uniform( "color", p.get_fill_color() );
	}
	else if ( p.has_fill_linear() )
	{
		m.set_program( new_program( ogl, vert, "linear_gradient.frag", false ) );
		m.get_program().use();
		m.set_uniform( "txt", 0 );
		m.set_uniform( "origin", p.get_fill_linear_origin() );
		m.set_uniform( "dir", p.get_fill_linear_size() );
	}
	else if ( p.has_fill_radial() )
	{
		m.set_program( new_program( ogl, vert, "radial_gradient.frag", false ) );
		m.get_program().use();
		m.set_uniform( "txt", 0 );
		m.set_uniform( "center", p.get_fill_radial_center() );
		m.set_uniform( "min_radius", p.get_fill_radial_r1() );
		m.set_uniform( "max_radius", p.get_fill_radial_r2() );
	}
	else if ( p.has_fill_conical() )
	{
		m.set_program( new_program( ogl, vert, "conical_gradient.frag", false ) );
		m.get_program().use();
		m.set_uniform( "txt", 0 );
		m.set_uniform( "center", p.get_fill_conical_center() );
	}
	else if ( p.has_fill_box() )
	{
		m.set_program( new_program( ogl, vert, "box_gradient.frag", false ) );
		m.get_program().use();
		m.set_uniform( "txt", 0 );
		m.set_uniform( "point1", p.get_fill_box_point1() );
		m.set_uniform( "point2", p.get_fill_box_point2() );
		m.set_uniform( "radius", p.get_fill_box_radius() );
	}
	else if ( p.has_no_fill() )
	{
		m.clear_program();
	}
	else
		throw std::runtime_error( "unhandled fill type" );

	if ( m.has_program() )
		return m.get_uniform_location( "matrix" );
	return -1;
}

////////////////////////////////////////

gl::program::uniform drawable::stroke_mesh( gl::api &ogl, gl::mesh &m, const paint &p, const std::string &vert )
{
	m.set_program( new_program( ogl, vert, "single_color.frag", false ) );
	m.get_program().use();
	m.set_uniform( "color", p.get_stroke_color() );
	return m.get_uniform_location( "matrix" );
}

////////////////////////////////////////

}

