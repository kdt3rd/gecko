
#pragma once

#include "opengl.h"
#include "enums.h"
#include <limits>
#include <base/color.h>
#include <base/contract.h>

namespace gl
{

class context;

////////////////////////////////////////

/// @brief OpenGL bound texture
class bound_texture
{
public:
	bound_texture( void )
	{
	}

	bound_texture( const bound_texture &other ) = delete;

	bound_texture( bound_texture &&other )
		: _target( other._target )
	{
		other._target = 0;
	}

	void image_2d_red( format f, size_t w, size_t h, image_type type, const void *data )
	{
		glTexImage2D( _target, 0, static_cast<GLint>(f), w, h, 0, GL_RED, static_cast<GLenum>(type), data );
	}

	void image_2d_rgb( format f, size_t w, size_t h, image_type type, const void *data )
	{
		glTexImage2D( _target, 0, static_cast<GLint>(f), w, h, 0, GL_RGB, static_cast<GLenum>(type), data );
	}

	void image_2d_rgba( format f, size_t w, size_t h, image_type type, const void *data )
	{
		glTexImage2D( _target, 0, static_cast<GLint>(f), w, h, 0, GL_RGBA, static_cast<GLenum>(type), data );
	}

	void subimage_2d( format f, int x, int y, size_t w, size_t h, image_type type, const void *data )
	{
		glTexSubImage2D( _target, 0, GLint(x), GLint(y), GLsizei(w), GLsizei(h),
						 static_cast<GLenum>(f), static_cast<GLenum>(type), data );
	}

	void set_wrapping( wrapping w )
	{
		glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>(w) );
		glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>(w) );
	}

	void set_wrapping( wrapping s, wrapping t )
	{
		glTexParameteri( _target, GL_TEXTURE_WRAP_S, static_cast<GLint>(s) );
		glTexParameteri( _target, GL_TEXTURE_WRAP_T, static_cast<GLint>(t) );
	}

	void set_filters( filter min, filter mag )
	{
		glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min) );
		glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag) );
	}

	void set_border_color( const base::color &c )
	{

		float color[] = { float(c.red()), float(c.green()), float(c.blue()), float(c.alpha()) };
		glTexParameterfv( _target, GL_TEXTURE_BORDER_COLOR, color );
	}

	void set_swizzle( swizzle r, swizzle g, swizzle b, swizzle a = swizzle::ONE )
	{
		GLint mask[4] = { static_cast<GLint>(r), static_cast<GLint>(g), static_cast<GLint>(b), static_cast<GLint>(a) };
		glTexParameteriv( _target, GL_TEXTURE_SWIZZLE_RGBA, mask );
	}

	void mipmap( void )
	{
//		glGenerateMipmap( _target );
	}

	void operator=( bound_texture &&t )
	{
		_target = t._target;
		t._target = 0;
	}

private:
	friend class texture;

	bound_texture( GLenum targ, GLuint txt )
		: _target( targ )
	{
		glBindTexture( _target, txt );
	}

	GLenum _target = 0;
};


////////////////////////////////////////

/// @brief OpenGL texture
class texture
{
public:
	enum class target
	{
		TEXTURE_2D = GL_TEXTURE_2D,
		TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE,
		TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	};

	texture( void );

	texture( GLuint t );

	texture( texture &&other );

	texture( const texture &other ) = delete;

	~texture( void );

	inline bool is_valid( void ) const
	{
		return _texture > 0;
	}

	inline bound_texture bind( target targ )
	{
		precondition( is_valid(), "invalid texture" );
		return bound_texture( static_cast<GLenum>( targ ), _texture );
	}

	inline bound_texture bind( target targ, int unit )
	{
		glActiveTexture( GL_TEXTURE0 + unit );
		return bind( targ );
	}

	GLuint id( void ) const
	{
		precondition( is_valid(), "invalid texture" );
		return _texture;
	}

private:
	GLuint _texture = 0;
};

////////////////////////////////////////

}

