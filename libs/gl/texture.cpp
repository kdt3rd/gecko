//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "texture.h"
#include <limits>

namespace
{
const constexpr size_t invalid_unit = std::numeric_limits<size_t>::max();

// TODO: thread safety???
static std::vector<gl::texture::binding*> theBound;
}

namespace gl
{

////////////////////////////////////////

texture::binding::binding( void )
	: _unit( invalid_unit ), _target( GL_TEXTURE_2D )
{
}

////////////////////////////////////////

texture::binding::binding( binding &&other )
	: _unit( other._unit ), _target( other._target )
{
	precondition( theBound[_unit] == &other, "bound texture lost unit" );
	other._unit = invalid_unit;
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
			std::cerr << "bound texture lost texture unit " << _unit << std::endl;
	}
}

////////////////////////////////////////

void texture::binding::image_2d_red( format f, size_t w, size_t h, image_type type, const void *data )
{
	glTexImage2D( _target, 0, static_cast<GLint>(f), static_cast<GLsizei>(w),
				  static_cast<GLsizei>(h), 0, GL_RED, static_cast<GLenum>(type), data );
}

////////////////////////////////////////

void texture::binding::image_2d_rgb( format f, size_t w, size_t h, image_type type, const void *data )
{
	glTexImage2D( _target, 0, static_cast<GLint>(f), static_cast<GLsizei>(w),
				  static_cast<GLsizei>(h), 0, GL_RGB, static_cast<GLenum>(type), data );
}

////////////////////////////////////////

void texture::binding::image_2d_rgba( format f, size_t w, size_t h, image_type type, const void *data )
{
	glTexImage2D( _target, 0, static_cast<GLint>(f), static_cast<GLsizei>(w),
				  static_cast<GLsizei>(h), 0, GL_RGBA, static_cast<GLenum>(type), data );
}

////////////////////////////////////////

void texture::binding::subimage_2d( format f, int x, int y, size_t w, size_t h, image_type type, const void *data )
{
	glTexSubImage2D( _target, 0, static_cast<GLint>(x), static_cast<GLint>(y),
					 static_cast<GLsizei>(w), static_cast<GLsizei>(h),
					 static_cast<GLenum>(f), static_cast<GLenum>(type), data );
}

////////////////////////////////////////

void texture::binding::set_wrapping( wrapping w )
{
	glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>(w) );
	glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>(w) );
}

////////////////////////////////////////

void texture::binding::set_wrapping( wrapping s, wrapping t )
{
	glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>(s) );
	glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>(t) );
}

////////////////////////////////////////

void texture::binding::set_filters( filter min, filter mag )
{
	glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min) );
	glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag) );
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
	GLint mask[4] = { static_cast<GLint>(r), static_cast<GLint>(g), static_cast<GLint>(b), static_cast<GLint>(a) };
	glTexParameteriv( _target, GL_TEXTURE_SWIZZLE_RGBA, mask );
}

////////////////////////////////////////

void texture::binding::operator=( binding &&other )
{
	precondition( _unit != invalid_unit, "invalid texture unit" );
	precondition( theBound[_unit] == &other, "bound texture lost unit" );
	other._unit = invalid_unit;
	theBound[_unit] = this;
}

////////////////////////////////////////

}

