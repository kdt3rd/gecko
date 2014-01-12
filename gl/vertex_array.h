
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

class bound_array
{
public:
	bound_array( void ) = delete;
	bound_array( const bound_array &other ) = delete;

	bound_array( bound_array &&other )
	{
	}

	~bound_array( void )
	{
		glBindVertexArray( 0 );
	}

	template<typename D>
	void attrib_pointer( program::attribute attr, std::shared_ptr<buffer<D>> &buf, size_t components, size_t stride = 0, size_t offset = 0 )
	{
		{
			auto bb = buf->bind( gl::target::ARRAY_BUFFER );
			glEnableVertexAttribArray( attr );
			glVertexAttribPointer( attr, components, gl_data_type<D>::value, GL_FALSE, stride * sizeof(D), reinterpret_cast<const GLvoid *>( offset * sizeof(D) ) );
		}
	}

	void draw( primitive prim, size_t start, size_t count )
	{
		glDrawArrays( static_cast<GLenum>( prim ), start, count );
	}

private:
	friend class context;
	friend class vertex_array;

	bound_array( GLuint arr )
	{
		glBindVertexArray( arr );
	}
};

////////////////////////////////////////

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

