//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "program.h"
#include <stdexcept>

namespace gl
{

////////////////////////////////////////

program *program::_using = nullptr;

////////////////////////////////////////

program::program( void )
{
	_program = glCreateProgram();
}

////////////////////////////////////////

program::~program( void )
{
	if ( _using == this )
	{
		_using = nullptr;
		glUseProgram( 0 );
	}
	glDeleteProgram( _program );
}

////////////////////////////////////////

void program::attach( const std::shared_ptr<shader> &shader )
{
	glAttachShader( _program, shader->_shader );
}

////////////////////////////////////////

void program::link( void )
{
	glLinkProgram( _program );

	GLint res;
	glGetProgramiv( _program, GL_LINK_STATUS, &res );

	if ( res == GL_FALSE )
		throw std::runtime_error( log() );
}

////////////////////////////////////////

void program::use( void )
{
	_using = this;
	glUseProgram( _program );
}

////////////////////////////////////////

std::string program::log( void )
{
	GLint res = 0;
	glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &res );

	if ( res > 0 )
	{
		std::string info( static_cast<size_t>( res ), 0 );
		glGetProgramInfoLog( _program, res, &res, &info[0] );
		return info;
	}
	return std::string();
}

////////////////////////////////////////

program::attribute program::get_attribute_location( const std::string &name )
{
	GLint loc = glGetAttribLocation( _program, name.c_str() );
	if ( loc < 0 )
		throw_runtime( "gl attribute {0} not found", name );
	return static_cast<program::attribute>( loc );
}

////////////////////////////////////////

program::uniform program::get_uniform_location( const std::string &name )
{
	GLint loc = glGetUniformLocation( _program, name.c_str() );
	if ( loc < 0 )
		throw_runtime( "gl uniform {0} not found", name );
	return static_cast<program::uniform>( loc );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, int value )
{
	precondition( _using == this, "program not in use" );
	glUniform1i( static_cast<GLint>( uni ), value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, float value )
{
	precondition( _using == this, "program not in use" );
	glUniform1f( static_cast<GLint>( uni ), value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, double value )
{
	precondition( _using == this, "program not in use" );
	glUniform1f( static_cast<GLint>( uni ), static_cast<GLfloat>( value ) );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const matrix4 &value )
{
	precondition( _using == this, "program not in use" );
	glUniformMatrix4fv( static_cast<GLint>( uni ), 1, GL_FALSE, value.data() );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const color &value )
{
	precondition( _using == this, "program not in use" );
	glUniform4f( static_cast<GLint>( uni ), value.red(), value.green(), value.blue(), value.alpha() );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec4 &value )
{
	precondition( _using == this, "program not in use" );
	glUniform4f( static_cast<GLint>( uni ), value[0], value[1], value[2], value[3] );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec3 &value )
{
	precondition( _using == this, "program not in use" );
	glUniform3f( static_cast<GLint>( uni ), value[0], value[1], value[2] );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec2 &value )
{
	precondition( _using == this, "program not in use" );
	glUniform2f( static_cast<GLint>( uni ), value[0], value[1] );
}

////////////////////////////////////////

size_t program::number_active_uniforms( void )
{
	GLint n;
	glGetProgramiv( _program, GL_ACTIVE_UNIFORMS, &n );
	return size_t( n );
}

////////////////////////////////////////

std::pair<uniform_type,std::string> program::active_uniform( size_t i )
{
	GLint max;
	glGetProgramiv( _program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max );

	std::string name( static_cast<size_t>( max ), '\0' );
	GLsizei len = 0;
	GLint sz = 0;
	GLenum type;
	glGetActiveUniform( _program, static_cast<GLuint>( i ), max, &len, &sz, &type, &name[0] );
	name.resize( static_cast<size_t>( len ) );

	uniform_type t;
	switch ( type )
	{
		case GL_FLOAT: t = uniform_type::FLOAT; break;
		case GL_FLOAT_VEC2: t = uniform_type::FLOAT_VEC2; break;
		case GL_FLOAT_VEC4: t = uniform_type::FLOAT_VEC4; break;
		case GL_FLOAT_MAT4: t = uniform_type::FLOAT_MAT4; break;
		default: t = uniform_type::OTHER; break;
	}

	return { t, name };
}

////////////////////////////////////////

}
