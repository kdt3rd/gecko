
#include "vertex_array.h"

namespace gl
{

////////////////////////////////////////

vertex_array::binding *vertex_array::binding::_bound = nullptr;

////////////////////////////////////////

vertex_array::binding::binding( binding &&o )
{
	if ( _bound == &o )
		_bound = this;
}

////////////////////////////////////////

vertex_array::binding &vertex_array::binding::operator=( binding &&o )
{
	if ( _bound == &o )
		_bound = this;
	return *this;
}

////////////////////////////////////////

vertex_array::binding::~binding( void )
{
	if ( _bound == this )
		glBindVertexArray( 0 );
}

////////////////////////////////////////

void vertex_array::binding::attrib_pointer( program::attribute attr, std::shared_ptr<vertex_buffer> &vbo, size_t components, size_t stride, size_t offset )
{
	auto bb = vbo->bind();
	glEnableVertexAttribArray( attr );
	glVertexAttribPointer( attr, static_cast<GLint>(components), GL_FLOAT, GL_FALSE, static_cast<GLsizei>( stride * sizeof(float) ), reinterpret_cast<const GLvoid *>( offset * sizeof(float) ) );
}

////////////////////////////////////////

void vertex_array::binding::draw( primitive prim, size_t start, size_t count )
{
	glDrawArrays( static_cast<GLenum>( prim ), static_cast<GLint>( start ), static_cast<GLsizei>( count ) );
}

////////////////////////////////////////

vertex_array::binding::binding( GLuint arr )
{
	glBindVertexArray( arr );
	_bound = this;
}

////////////////////////////////////////

vertex_array::vertex_array( void )
{
	glGenVertexArrays( 1, &_array );
}

////////////////////////////////////////

vertex_array::~vertex_array( void )
{
	glDeleteVertexArrays( 1, &_array );
}

////////////////////////////////////////

vertex_array::binding vertex_array::bind( void )
{
	return binding( _array );
}

////////////////////////////////////////

}

