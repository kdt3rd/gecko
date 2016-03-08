
#pragma once

#include "opengl.h"
#include "enums.h"
#include <vector>

namespace gl
{

class api;

////////////////////////////////////////

/// @brief Buffer usage
enum class buffer_usage
{
	STREAM_DRAW = GL_STREAM_DRAW,
	STATIC_DRAW = GL_STATIC_DRAW,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
};

////////////////////////////////////////

/// @brief Buffer target
enum class buffer_target
{
	/// Buffer of vertex attributes
	ARRAY_BUFFER = GL_ARRAY_BUFFER,

	/// Buffer of vertex indices
	ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
};

////////////////////////////////////////

template<typename D> class buffer;

/// @brief OpenGL bound buffer
template<typename D>
class bound_buffer
{
public:
	bound_buffer( void ) = delete;
	bound_buffer( const bound_buffer &other ) = delete;

	bound_buffer( bound_buffer &&other )
		: _target( other._target )
	{
		other._target = 0;
	}

	~bound_buffer( void )
	{
		if ( _target != 0 )
			glBindBuffer( _target, 0 );
	}

	void data( const D *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		glBufferData( _target, n * sizeof(D), data, static_cast<GLenum>( u ) );
	}

	void data( const std::vector<D> &data, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		glBufferData( _target, static_cast<GLsizeiptr>( data.size() * sizeof(D) ), data.data(), static_cast<GLenum>( u ) );
	}

	void sub_data( const D *data, size_t offset, size_t n )
	{
		glBufferSubData( _target, offset * sizeof(D), n * sizeof(D), data );
	}

	void sub_data( const std::vector<D> &data, size_t offset )
	{
		glBufferSubData( _target, offset * sizeof(D), data.size() * sizeof(D), data.data() );
	}

	void draw( primitive prim, size_t n )
	{
		glDrawElements( static_cast<GLenum>( prim ), static_cast<GLsizei>( n ), gl_data_type<D>::value, nullptr );
	}

private:
	friend class buffer<D>;
	friend class api;

	bound_buffer( GLenum targ, GLuint buf )
		: _target( targ )
	{
		glBindBuffer( _target, buf );
	}

	GLenum _target;
};

////////////////////////////////////////

/// @brief OpenGL buffer
template<typename D>
class buffer
{
public:
	buffer( const buffer &b ) = delete;

	buffer( buffer_target t )
		: _target( static_cast<GLenum>( t ) )
	{
		glGenBuffers( 1, &_buffer );
	}

	buffer( buffer_target targ, const D *data, size_t n, buffer_usage u )
		: buffer( targ )
	{
		auto bb = bind();
		bb.data( data, n, u );
	}

	buffer( buffer_target targ, const std::vector<D> &data, buffer_usage u )
		: buffer( targ )
	{
		auto bb = bind();
		bb.data( data, u );
	}

	~buffer( void )
	{
		glDeleteBuffers( 1, &_buffer );
	}

	bound_buffer<D> bind( void )
	{
		return bound_buffer<D>( static_cast<GLenum>( _target ), _buffer );
	}

private:
	friend class api;

	GLuint _buffer;
	GLenum _target;
};

////////////////////////////////////////

}

