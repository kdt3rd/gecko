
#pragma once

#include "opengl.h"
#include "enums.h"
#include "program.h"
#include "buffer.h"
#include <vector>

namespace gl
{

class vertex_array;

////////////////////////////////////////

/// @brief OpenGL bound vertex array
class bound_array
{
public:
	bound_array( void ) = delete;
	bound_array( const bound_array & ) = delete;
	bound_array &operator=( const bound_array & ) = delete;
	bound_array &operator=( bound_array &&o )
	{
		_a = 0;
		std::swap( _a, o._a );
		return *this;
	}
	bound_array( bound_array &&o )
		: _a( o._a )
	{
		o._a = 0;
	}

	~bound_array( void )
	{
		if ( _a )
			glBindVertexArray( 0 );
	}

	template<typename D>
	void attrib_pointer( program::attribute attr, std::shared_ptr<buffer<D>> &buf, size_t components, size_t stride = 0, size_t offset = 0 )
	{
		{
			auto bb = buf->bind( gl::buffer<D>::target::ARRAY_BUFFER );
			glEnableVertexAttribArray( attr );
			glVertexAttribPointer( attr, static_cast<GLint>(components), gl_data_type<D>::value, GL_FALSE, static_cast<GLsizei>( stride * sizeof(D) ), reinterpret_cast<const GLvoid *>( offset * sizeof(D) ) );
		}
	}

	template<typename D, typename E>
	void attrib_pointer( program::attribute attr, std::shared_ptr<buffer<D>> &buf, const std::vector<E> &components, size_t nPer, size_t stride = 0, size_t offset = 0 )
	{
		{
			auto bb = buf->bind( gl::buffer<D>::target::ARRAY_BUFFER );
			bb.data( components, gl::usage::STATIC_DRAW );
			glEnableVertexAttribArray( attr );
			stride *= sizeof(D);
			offset *= sizeof(D);
			glVertexAttribPointer( attr, static_cast<GLint>(nPer), gl_data_type<D>::value, GL_FALSE, static_cast<GLsizei>( stride ), reinterpret_cast<const GLvoid *>( offset ) );
		}
	}

	void draw( primitive prim, size_t start, size_t count )
	{
		glDrawArrays( static_cast<GLenum>( prim ), static_cast<GLint>( start ), static_cast<GLsizei>( count ) );
	}

	template <typename T>
	void draw_indices( primitive prim, const std::vector<T> &buf )
	{
		glDrawElements( static_cast<GLenum>( prim ), buf.size(),
						gl_data_type<T>::value,
						buf.data() );
	}

private:
	friend class context;
	friend class vertex_array;

	GLuint _a;
	bound_array( GLuint arr )
			: _a( arr )
	{
		glBindVertexArray( arr );
	}
};

////////////////////////////////////////

/// @brief OpenGL vertex array
class vertex_array
{
public:
	vertex_array( const vertex_array &b ) = delete;

	vertex_array( void )
	{
		glGenVertexArrays( 1, &_array );
	}

	~vertex_array( void )
	{
		glDeleteVertexArrays( 1, &_array );
	}

	bound_array bind( void )
	{
		return bound_array( _array );
	}

private:
	friend class context;

	GLuint _array;
};

////////////////////////////////////////

}

