
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

program::program( const std::shared_ptr<shader> &vertex )
	: program()
{
	attach( vertex );
	link();
	glUseProgram( _program );
}

////////////////////////////////////////

program::program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment )
	: program()
{
	attach( vertex );
	attach( fragment );
	link();
	glUseProgram( _program );
}

////////////////////////////////////////

program::program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment, const std::shared_ptr<shader> &geometry )
	: program()
{
	attach( vertex );
	attach( fragment );
	attach( geometry );
	link();
	glUseProgram( _program );
}

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

std::string program::log( void )
{
	GLint res;
	glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &res );

	if ( res > 0 )
	{
		std::string info( res, 0 );
		glGetProgramInfoLog( _program, res, &res, &info[0] );
		return info;
	}
	return std::string();
}

////////////////////////////////////////

program::attribute program::get_attribute_location( const std::string &name )
{
	return glGetAttribLocation( _program, name.c_str() );
}

////////////////////////////////////////

program::uniform program::get_uniform_location( const std::string &name )
{
	return glGetUniformLocation( _program, name.c_str() );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, int value )
{
	glUniform1i( uni, value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, float value )
{
	glUniform1f( uni, value );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const matrix4 &value )
{
	glUniformMatrix4fv( uni, 1, GL_FALSE, value.data() );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const core::color &value )
{
	float tmp[] = { float(value.red()), float(value.green()), float(value.blue()), float(value.alpha()) };
	glUniform4fv( uni, 1, tmp );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const core::point &value )
{
	float tmp[] = { float(value.x()), float(value.y()) };
	glUniform2fv( uni, 1, tmp );
}

////////////////////////////////////////

void program::set_uniform( uniform uni, const core::size &value )
{
	float tmp[] = { float(value.w()), float(value.h()) };
	glUniform2fv( uni, 1, tmp );
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

	std::string name( max, '\0' );
	GLsizei len = 0;
	GLint sz = 0;
	GLenum type;
	glGetActiveUniform( _program, i, max, &len, &sz, &type, &name[0] );
	name.resize( len );

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
