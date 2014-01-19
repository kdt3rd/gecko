
#pragma once

#include "opengl.h"
#include "enums.h"
#include <limits>

namespace gl
{

class context;

////////////////////////////////////////

class bound_texture
{
public:
	bound_texture( void ) = delete;
	bound_texture( const bound_texture &other ) = delete;
	bound_texture( bound_texture &&other )
		: _target( other._target )
	{
		other._target = 0;
	}

	void image_2d( format f, size_t w, size_t h, image_type type, const void *data )
	{
		glTexImage2D( _target, 0, static_cast<GLint>(f), w, h, 0, static_cast<GLenum>(f), static_cast<GLenum>(type), data );
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

	void mipmap( void )
	{
//		glGenerateMipmap( _target );
	}

private:
	friend class texture;

	bound_texture( GLenum targ, GLuint txt )
		: _target( targ )
	{
		glBindTexture( _target, txt );
	}

	GLenum _target;
};


////////////////////////////////////////

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

	texture( const texture &other ) = delete;

	~texture( void );

	inline bound_texture bind( target targ )
	{
		return bound_texture( static_cast<GLenum>( targ ), _texture );
	}

	inline bound_texture bind( target targ, int unit )
	{
		glActiveTexture( GL_TEXTURE0 + unit );
		return bind( targ );
	}

private:
	GLuint _texture;
};

////////////////////////////////////////

}

