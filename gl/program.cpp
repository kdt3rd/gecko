
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

program::~program()
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

/*
void program::SetUniform( const Uniform& uniform, const Vec2& value )
{
	glUniform2f( uniform, value.X, value.Y );
}

void program::SetUniform( const Uniform& uniform, const Vec3& value )
{
	glUniform3f( uniform, value.X, value.Y, value.Z );
}

void program::SetUniform( const Uniform& uniform, const Vec4& value )
{
	glUniform4f( uniform, value.X, value.Y, value.Z, value.W );
}

void program::SetUniform( const Uniform& uniform, const float* values, uint count )
{
	glUniform1fv( uniform, count, values );
}

void program::SetUniform( const Uniform& uniform, const Vec2* values, uint count )
{
	glUniform2fv( uniform, count, (float*)values );
}

void program::SetUniform( const Uniform& uniform, const Vec3* values, uint count )
{
	glUniform3fv( uniform, count, (float*)values );
}

void program::SetUniform( const Uniform& uniform, const Vec4* values, uint count )
{
	glUniform4fv( uniform, count, (float*)values );
}

void program::SetUniform( const Uniform& uniform, const Mat3& value )
{
	glUniformMatrix3fv( uniform, 1, GL_FALSE, value.m );
}

void program::SetUniform( const Uniform& uniform, const Mat4& value )
{
	glUniformMatrix4fv( uniform, 1, GL_FALSE, value.m );
}

*/

}
