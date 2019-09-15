// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "texture.h"

#include <limits>

namespace
{
const constexpr size_t invalid_unit = std::numeric_limits<size_t>::max();

// TODO: thread safety???
static std::vector<gl::texture::binding *> theBound;
} // namespace

namespace gl
{
////////////////////////////////////////

texture::binding::binding( void )
    : _unit( invalid_unit ), _target( GL_TEXTURE_2D )
{}

////////////////////////////////////////

texture::binding::binding( binding &&other )
    : _unit( other._unit ), _target( other._target )
{
    precondition( theBound[_unit] == &other, "bound texture lost unit" );
    other._unit     = invalid_unit;
    theBound[_unit] = this;
}

////////////////////////////////////////

texture::binding::binding( GLuint txt, size_t unit, GLenum target )
    : _unit( unit ), _target( target )
{
    precondition( _unit != invalid_unit, "invalid unit" );
    if ( _unit >= theBound.size() )
        theBound.resize( _unit + 1 );

    glActiveTexture( GL_TEXTURE0 + static_cast<GLenum>( unit ) );
    glBindTexture( _target, txt );

    if ( theBound[_unit] != nullptr )
        theBound[_unit]->_unit = invalid_unit;
    theBound[_unit] = this;
}

////////////////////////////////////////

texture::binding::~binding( void )
{
    if ( _unit != invalid_unit )
    {
        if ( theBound[_unit] == this )
        {
            glActiveTexture( GL_TEXTURE0 + static_cast<GLenum>( _unit ) );
            glBindTexture( _target, 0 );
            theBound[_unit] = nullptr;
        }
        else
            std::cerr << "bound texture lost texture unit " << _unit
                      << std::endl;
    }
}

////////////////////////////////////////

void texture::binding::upload_helper(
    format      f,
    GLenum      fmt,
    int         c,
    size_t      w,
    size_t      h,
    image_type  type,
    const void *data,
    size_t      stride_bytes,
    bool        needswap )
{
    GLint bytesper = 1;
    switch ( type )
    {
        case image_type::UNSIGNED_BYTE: bytesper = 1; break;
        case image_type::UNSIGNED_SHORT:
        case image_type::HALF: bytesper = 2; break;
        case image_type::UNSIGNED_INT:
        case image_type::FLOAT: bytesper = 4; break;
        case image_type::UNSIGNED_SHORT_4_4_4_4:
        case image_type::UNSIGNED_SHORT_5_5_5_1:
        case image_type::UNSIGNED_INT_10_10_10_2:
        case image_type::UNSIGNED_SHORT_5_6_5:
            //default:
            throw_runtime( "invalid image type" );
    }
    if ( stride_bytes != 0 )
    {
        GLint units = static_cast<GLint>(
            stride_bytes / static_cast<size_t>( c * bytesper ) );
        precondition(
            0 == ( stride_bytes - static_cast<size_t>( units ) *
                                      static_cast<size_t>( c * bytesper ) ),
            "expect alignment that is a multiple of a pixel unit" );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, units );
    }
    else
        glPixelStorei( GL_UNPACK_ROW_LENGTH, static_cast<GLint>( w ) );
    glPixelStorei( GL_UNPACK_SWAP_BYTES, needswap ? 1 : 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, bytesper );
    glTexImage2D(
        _target,
        0,
        static_cast<GLint>( f ),
        static_cast<GLsizei>( w ),
        static_cast<GLsizei>( h ),
        0,
        fmt,
        static_cast<GLenum>( type ),
        data );
    glPixelStorei( GL_UNPACK_SWAP_BYTES, 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
}

////////////////////////////////////////

void texture::binding::subimage_2d(
    format      f,
    int         x,
    int         y,
    size_t      w,
    size_t      h,
    image_type  type,
    const void *data,
    size_t      stride_bytes,
    bool        needswap )
{
    int c = 1;
    switch ( f )
    {
        case format::RED:
        case format::RED_HALF:
        case format::RED_FLOAT: c = 1; break;
        case format::RG:
        case format::RG_HALF:
        case format::RG_FLOAT: c = 2; break;
        case format::RGB:
        case format::RGB_HALF:
        case format::RGB_FLOAT: c = 3; break;
        case format::RGBA:
        case format::RGBA_HALF:
        case format::RGBA_FLOAT:
            c = 4;
            break;
            //default:
            throw_runtime( "unhandled format" );
    }
    GLint bytesper = 1;
    switch ( type )
    {
        case image_type::UNSIGNED_BYTE: bytesper = 1; break;
        case image_type::UNSIGNED_SHORT:
        case image_type::HALF: bytesper = 2; break;
        case image_type::UNSIGNED_INT:
        case image_type::FLOAT: bytesper = 4; break;
        case image_type::UNSIGNED_SHORT_4_4_4_4:
        case image_type::UNSIGNED_SHORT_5_5_5_1:
        case image_type::UNSIGNED_INT_10_10_10_2:
        case image_type::UNSIGNED_SHORT_5_6_5:
            //default:
            throw_runtime( "invalid image type" );
    }
    if ( stride_bytes != 0 )
    {
        GLint units = static_cast<GLint>(
            stride_bytes / static_cast<size_t>( c * bytesper ) );
        precondition(
            0 == ( stride_bytes - static_cast<size_t>( units ) *
                                      static_cast<size_t>( c * bytesper ) ),
            "expect alignment that is a multiple of a pixel unit" );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, units );
    }
    else
        glPixelStorei( GL_UNPACK_ROW_LENGTH, static_cast<GLint>( w ) );
    glPixelStorei( GL_UNPACK_SWAP_BYTES, needswap ? 1 : 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, bytesper );

    glTexSubImage2D(
        _target,
        0,
        static_cast<GLint>( x ),
        static_cast<GLint>( y ),
        static_cast<GLsizei>( w ),
        static_cast<GLsizei>( h ),
        static_cast<GLenum>( f ),
        static_cast<GLenum>( type ),
        data );

    glPixelStorei( GL_UNPACK_SWAP_BYTES, 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
}

////////////////////////////////////////

void texture::binding::set_wrapping( wrapping w )
{
    glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>( w ) );
    glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>( w ) );
    glTexParameteri( _target, GL_TEXTURE_WRAP_R, static_cast<GLint>( w ) );
}

////////////////////////////////////////

void texture::binding::set_wrapping( wrapping s, wrapping t )
{
    glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>( s ) );
    glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>( t ) );
    glTexParameteri( _target, GL_TEXTURE_WRAP_R, static_cast<GLint>( t ) );
}

////////////////////////////////////////

void texture::binding::set_filters( filter min, filter mag )
{
    glTexParameteri(
        _target, GL_TEXTURE_MIN_FILTER, static_cast<GLint>( min ) );
    glTexParameteri(
        _target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>( mag ) );
}

////////////////////////////////////////

void texture::binding::set_border_color( const color &c )
{
    float color[] = { c.red(), c.green(), c.blue(), c.alpha() };
    glTexParameterfv( _target, GL_TEXTURE_BORDER_COLOR, color );
}

////////////////////////////////////////

void texture::binding::set_swizzle( swizzle r, swizzle g, swizzle b, swizzle a )
{
    GLint mask[4] = { static_cast<GLint>( r ),
                      static_cast<GLint>( g ),
                      static_cast<GLint>( b ),
                      static_cast<GLint>( a ) };
    glTexParameteriv( _target, GL_TEXTURE_SWIZZLE_RGBA, mask );
}

////////////////////////////////////////

void texture::binding::operator=( binding &&other )
{
    precondition( other._unit != invalid_unit, "invalid texture unit" );
    precondition( theBound[other._unit] == &other, "bound texture lost unit" );
    _unit           = other._unit;
    other._unit     = invalid_unit;
    theBound[_unit] = this;
}

////////////////////////////////////////

} // namespace gl
