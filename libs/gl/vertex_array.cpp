//
// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT
//

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
	precondition( _bound == this, "vertex array not bound" );

	_self->_vbos.push_back( vbo );
	auto bb = vbo->bind();
	glEnableVertexAttribArray( attr );
	glVertexAttribPointer( attr, static_cast<GLint>(components), GL_FLOAT, GL_FALSE, static_cast<GLsizei>( stride * sizeof(float) ), reinterpret_cast<const GLvoid *>( offset * sizeof(float) ) );
}

////////////////////////////////////////

void vertex_array::binding::bind_elements( const std::shared_ptr<element_buffer> &ebo )
{
	_self->_ebo = ebo;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo->id() );
}

////////////////////////////////////////

void vertex_array::binding::draw_arrays( primitive prim, size_t start, size_t count )
{
	precondition( _bound == this, "vertex array not bound" );
	glDrawArrays( static_cast<GLenum>( prim ), static_cast<GLint>( start ), static_cast<GLsizei>( count ) );
}

////////////////////////////////////////

void vertex_array::binding::draw_elements( primitive prim, size_t start, size_t count )
{
	precondition( _bound == this, "vertex array not bound" );
	precondition( _self->has_element_buffer(), "need element buffer object to draw" );
	glDrawElements( static_cast<GLenum>( prim ), static_cast<GLsizei>( count ), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid *>( start * sizeof(uint32_t) ) );
}

////////////////////////////////////////

vertex_array::binding::binding( vertex_array *self )
	: _self( self )
{
	glBindVertexArray( _self->id() );
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
	return binding( this );
}

////////////////////////////////////////

}

