
#include "vertex_buffer.h"

namespace gl
{

////////////////////////////////////////

vertex_buffer::binding *vertex_buffer::binding::_bound = nullptr;

////////////////////////////////////////

vertex_buffer::binding::binding( binding &&other )
{
	if ( _bound == &other )
		_bound = this;
}

////////////////////////////////////////

vertex_buffer::binding::~binding( void )
{
	if ( _bound == this )
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

////////////////////////////////////////

void vertex_buffer::binding::data( const float *data, size_t n, buffer_usage u )
{
	precondition( _bound == this, "vertex buffer not bound" );
	glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( n * sizeof(float) ), data, static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void vertex_buffer::binding::data( const std::vector<float> &data, buffer_usage u )
{
	precondition( _bound == this, "vertex buffer not bound" );
	glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( data.size() * sizeof(float) ), data.data(), static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void vertex_buffer::binding::sub_data( const float *data, size_t offset, size_t n )
{
	precondition( _bound == this, "vertex buffer not bound" );
	glBufferSubData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( offset * sizeof(float) ), static_cast<GLsizeiptr>( n * sizeof(float) ), data );
}

////////////////////////////////////////

void vertex_buffer::binding::sub_data( const std::vector<float> &data, size_t offset )
{
	precondition( _bound == this, "vertex buffer not bound" );
	glBufferSubData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( offset * sizeof(float) ), static_cast<GLsizeiptr>( data.size() * sizeof(float) ), data.data() );
}

////////////////////////////////////////

void vertex_buffer::binding::draw( primitive prim, size_t n )
{
	precondition( _bound == this, "vertex buffer not bound" );
	glDrawElements( static_cast<GLenum>( prim ), static_cast<GLsizei>( n ), gl_data_type<float>::value, nullptr );
}

////////////////////////////////////////

void vertex_buffer::binding::operator=( binding &&other )
{
	if ( _bound == &other )
		_bound = this;
}

////////////////////////////////////////

vertex_buffer::binding::binding( GLuint buf )
{
	glBindBuffer( GL_ARRAY_BUFFER, buf );
	_bound = this;
}

////////////////////////////////////////

vertex_buffer::vertex_buffer( void )
{
	glGenBuffers( 1, &_buffer );
}

////////////////////////////////////////

vertex_buffer::vertex_buffer( const float *data, size_t n, buffer_usage u )
	: vertex_buffer()
{
	auto bb = bind();
	bb.data( data, n, u );
}

////////////////////////////////////////

vertex_buffer::vertex_buffer( const std::vector<float> &data, buffer_usage u )
	: vertex_buffer()
{
	auto bb = bind();
	bb.data( data, u );
}

////////////////////////////////////////

vertex_buffer::~vertex_buffer( void )
{
	glDeleteBuffers( 1, &_buffer );
}

////////////////////////////////////////

vertex_buffer::binding vertex_buffer::bind( void )
{
	return binding( _buffer );
}

////////////////////////////////////////

}

