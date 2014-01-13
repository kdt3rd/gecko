
#include <vector>
#include <iostream>
#include <stdexcept>
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
	const char *c = code.c_str();
	glShaderSource( _shader, 1, &c, NULL );
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
		std::string source( len + 1, '\0' );
		glGetShaderSource( _shader, len + 1, nullptr, &source[0] );
		std::cerr << "Compile error:\n" << source << std::endl;
		throw std::runtime_error( log() );
	}
}

////////////////////////////////////////

std::string shader::log( void )
{
	GLint res;
	glGetShaderiv( _shader, GL_INFO_LOG_LENGTH, &res );

	if ( res > 0 )
	{
		std::string info( res, 0 );
		glGetShaderInfoLog( _shader, res, &res, &info[0] );
		return info;
	}
	return std::string();
}

////////////////////////////////////////

}
