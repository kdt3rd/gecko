//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <vector>
#include <iostream>
#include <stdexcept>
#include <base/string_util.h>
#include <base/contract.h>
#include "shader.h"

namespace gl
{

////////////////////////////////////////

shader::shader( type t )
{
	_shader = glCreateShader( static_cast<GLenum>( t ) );
}

////////////////////////////////////////

shader::shader( type t, const std::string &code )
	: shader( t )
{
	source( code );
	compile();
}

////////////////////////////////////////

shader::~shader( void )
{
	glDeleteShader( _shader );
}

////////////////////////////////////////

void shader::source( const std::string &code )
{
	precondition( ! code.empty(), "attempt to compile empty shader source" );

	const char *c = code.c_str();
	glShaderSource( _shader, 1, &c, nullptr );
}

////////////////////////////////////////

void shader::compile( void )
{
	glCompileShader( _shader );

	GLint res;
	glGetShaderiv( _shader, GL_COMPILE_STATUS, &res );

	if ( res == GL_FALSE )
	{
		GLint len = 0;
		glGetShaderiv( _shader, GL_SHADER_SOURCE_LENGTH, &len );
		std::string source( static_cast<size_t>( len ), '\0' );
		glGetShaderSource( _shader, len, nullptr, &source[0] );
		std::cerr << "Compile error in:\n'\n" << source << "\n':" << std::endl;
		throw std::runtime_error( log() );
	}
	else
	{
		// seems like the log is uninitialized on some drivers if
		// there is a success...
//		std::string l = base::trim( log() );
//		if ( !l.empty() )
//		{
//			GLint len = 0;
//			glGetShaderiv( _shader, GL_SHADER_SOURCE_LENGTH, &len );
//			std::string source( static_cast<size_t>( len + 1 ), '\0' );
//			glGetShaderSource( _shader, len+1, nullptr, &source[0] );
//			std::clog << "========================================" << std::endl;
//			std::clog << "shader source len: " << len << std::endl;
//			std::clog << "Compile warning:\n" << source << std::endl;
//			std::clog << "----------------------------------------" << std::endl;
//			std::clog << l << std::endl;
//			std::clog << "========================================" << std::endl;
//		}
	}
}

////////////////////////////////////////

std::string shader::log( void )
{
	GLint res = 0;
	glGetShaderiv( _shader, GL_INFO_LOG_LENGTH, &res );

	if ( res > 1 )
	{
		GLsizei sz = res;
		GLsizei rsz = sz;
		std::string info( static_cast<size_t>( res + 1 ), 0 );
		glGetShaderInfoLog( _shader, sz, &rsz, &info[0] );
		return info;
	}
	return std::string();
}

////////////////////////////////////////

}
