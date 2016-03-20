
#include "index_buffer.h"

namespace gl
{

////////////////////////////////////////

index_buffer::binding *index_buffer::binding::_bound = nullptr;

////////////////////////////////////////

index_buffer::binding::binding( binding &&other )
{
	if ( _bound == &other )
		_bound = this;
}

////////////////////////////////////////

index_buffer::binding::~binding( void )
{
	if ( _bound == this )
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

////////////////////////////////////////

void index_buffer::binding::data( const uint32_t *data, size_t n, buffer_usage u )
{
	precondition( _bound == this, "index buffer not bound" );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( n * sizeof(uint32_t) ), data, static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void index_buffer::binding::data( const std::vector<uint32_t> &data, buffer_usage u )
{
	precondition( _bound == this, "index buffer not bound" );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( data.size() * sizeof(uint32_t) ), data.data(), static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void index_buffer::binding::sub_data( const uint32_t *data, size_t offset, size_t n )
{
	precondition( _bound == this, "index buffer not bound" );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( offset * sizeof(uint32_t) ), static_cast<GLsizeiptr>( n * sizeof(uint32_t) ), data );
}

////////////////////////////////////////

void index_buffer::binding::sub_data( const std::vector<uint32_t> &data, size_t offset )
{
	precondition( _bound == this, "index buffer not bound" );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( offset * sizeof(uint32_t) ), static_cast<GLsizeiptr>( data.size() * sizeof(uint32_t) ), data.data() );
}

////////////////////////////////////////

void index_buffer::binding::draw( primitive prim, size_t n )
{
	precondition( _bound == this, "index buffer not bound" );
	glDrawElements( static_cast<GLenum>( prim ), static_cast<GLsizei>( n ), gl_data_type<uint32_t>::value, nullptr );
}

////////////////////////////////////////

void index_buffer::binding::operator=( binding &&other )
{
	if ( _bound == &other )
		_bound = this;
}

////////////////////////////////////////

index_buffer::binding::binding( GLuint buf )
{
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buf );
	_bound = this;
}

////////////////////////////////////////

index_buffer::index_buffer( void )
{
	glGenBuffers( 1, &_buffer );
}

////////////////////////////////////////

index_buffer::index_buffer( const uint32_t *data, size_t n, buffer_usage u )
	: index_buffer()
{
	auto bb = bind();
	bb.data( data, n, u );
}

////////////////////////////////////////

index_buffer::index_buffer( const std::vector<uint32_t> &data, buffer_usage u )
	: index_buffer()
{
	auto bb = bind();
	bb.data( data, u );
}

////////////////////////////////////////

index_buffer::~index_buffer( void )
{
	glDeleteBuffers( 1, &_buffer );
}

////////////////////////////////////////

index_buffer::binding index_buffer::bind( void )
{
	return binding( _buffer );
}

////////////////////////////////////////

}

