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

std::shared_ptr<gl::program> drawable::new_program( gl::api &ogl, const std::string &vert, const std::string &frag )
{
	std::shared_ptr<gl::program> result;
	std::string lookup = vert + '|' + frag;

	auto cached = _program_cache.find( lookup );
	if ( cached != _program_cache.end() )
		result = cached->second.lock();

	if ( !result )
	{
		result = std::make_shared<gl::program>(
			ogl.new_shader( gl::shader::type::VERTEX, draw::shaders( vert ) ),
			ogl.new_shader( gl::shader::type::FRAGMENT, draw::shaders( frag ) )
		);
		_program_cache[lookup] = result;
	}

	return result;
}

////////////////////////////////////////

}

