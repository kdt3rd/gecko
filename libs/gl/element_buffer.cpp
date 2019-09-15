// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "element_buffer.h"

namespace gl
{
////////////////////////////////////////

element_buffer::binding *element_buffer::binding::_bound = nullptr;

////////////////////////////////////////

element_buffer::binding::binding( binding &&other )
{
    if ( _bound == &other )
        _bound = this;
}

////////////////////////////////////////

element_buffer::binding::~binding( void )
{
    if ( _bound == this )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        _bound = nullptr;
    }
}

////////////////////////////////////////

void element_buffer::binding::data(
    const uint32_t *data, size_t n, buffer_usage u )
{
    precondition( _bound == this, "element buffer not bound" );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>( n * sizeof( uint32_t ) ),
        data,
        static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void element_buffer::binding::data(
    const std::vector<uint32_t> &data, buffer_usage u )
{
    precondition( _bound == this, "element buffer not bound" );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>( data.size() * sizeof( uint32_t ) ),
        data.data(),
        static_cast<GLenum>( u ) );
}

////////////////////////////////////////

void element_buffer::binding::sub_data(
    const uint32_t *data, size_t offset, size_t n )
{
    precondition( _bound == this, "element buffer not bound" );
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>( offset * sizeof( uint32_t ) ),
        static_cast<GLsizeiptr>( n * sizeof( uint32_t ) ),
        data );
}

////////////////////////////////////////

void element_buffer::binding::sub_data(
    const std::vector<uint32_t> &data, size_t offset )
{
    precondition( _bound == this, "element buffer not bound" );
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>( offset * sizeof( uint32_t ) ),
        static_cast<GLsizeiptr>( data.size() * sizeof( uint32_t ) ),
        data.data() );
}

////////////////////////////////////////

void element_buffer::binding::draw( primitive prim, size_t n )
{
    precondition( _bound == this, "element buffer not bound" );
    glDrawElements(
        static_cast<GLenum>( prim ),
        static_cast<GLsizei>( n ),
        gl_data_type<uint32_t>::value,
        nullptr );
}

////////////////////////////////////////

void element_buffer::binding::operator=( binding &&other )
{
    if ( _bound == &other )
        _bound = this;
}

////////////////////////////////////////

element_buffer::binding::binding( GLuint buf )
{
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buf );
    _bound = this;
}

////////////////////////////////////////

element_buffer::element_buffer( void ) { glGenBuffers( 1, &_buffer ); }

////////////////////////////////////////

element_buffer::element_buffer( const uint32_t *data, size_t n, buffer_usage u )
    : element_buffer()
{
    auto bb = bind();
    bb.data( data, n, u );
}

////////////////////////////////////////

element_buffer::element_buffer(
    const std::vector<uint32_t> &data, buffer_usage u )
    : element_buffer()
{
    auto bb = bind();
    bb.data( data, u );
}

////////////////////////////////////////

element_buffer::~element_buffer( void ) { glDeleteBuffers( 1, &_buffer ); }

////////////////////////////////////////

element_buffer::binding element_buffer::bind( void )
{
    return binding( _buffer );
}

////////////////////////////////////////

} // namespace gl
