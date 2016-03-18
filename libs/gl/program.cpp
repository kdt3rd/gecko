
#include "program.h"
#include <stdexcept>

namespace gl
{

////////////////////////////////////////

program::program( void )
{
	_program = glCreateProgram();
}

////////////////////////////////////////

/*
program::program( const std::shared_ptr<shader> &vertex )
	: program()
{
	attach( vertex );
	link();
}

////////////////////////////////////////

program::program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment )
	: program()
{
	attach( vertex );
	attach( fragment );
	link();
}

////////////////////////////////////////

program::program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment, const std::shared_ptr<shader> &geometry )
	: program()
{
	attach( vertex );
	attach( fragment );
	attach( geometry );
	link();
}
*/

////////////////////////////////////////

program::~program( void )
{
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
	glUseProgram( _program );
}

////////////////////////////////////////

std::string program::log( void )
{
	GLint res;
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
	return static_cast<program::attribute>( glGetAttribLocation( _program, name.c_str() ) );
}

////////////////////////////////////////

program::uniform program::get_uniform_location( const std::string &name )
{
	return static_cast<program::uniform>( glGetUniformLocation( _program, name.c_str() ) );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, int value )
{
	glUniform1i( static_cast<GLint>( uni ), value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, float value )
{
	glUniform1f( static_cast<GLint>( uni ), value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, double value )
{
	glUniform1f( static_cast<GLint>( uni ), static_cast<GLfloat>( value ) );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const matrix4 &value )
{
	glUniformMatrix4fv( static_cast<GLint>( uni ), 1, GL_FALSE, value.data() );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const color &value )
{
	float tmp[] = { value.red(), value.green(), value.blue(), value.alpha() };
	glUniform4fv( static_cast<GLint>( uni ), 1, tmp );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec4 &value )
{
	float tmp[] = { value[0], value[1], value[2], value[3] };
	glUniform4fv( static_cast<GLint>( uni ), 1, tmp );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec3 &value )
{
	float tmp[] = { value[0], value[1], value[2] };
	glUniform3fv( static_cast<GLint>( uni ), 1, tmp );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const vec2 &value )
{
	float tmp[] = { value[0], value[1] };
	glUniform2fv( static_cast<GLint>( uni ), 1, tmp );
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
