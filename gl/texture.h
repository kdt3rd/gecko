
#pragma once

#include "opengl.h"
#include "enums.h"
#include <limits>

namespace gl
{

class context;

////////////////////////////////////////

class texture
{
public:
	enum class target
	{
		TEXTURE_2D = GL_TEXTURE_2D,
		TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	};

	texture( const texture &other ) = delete;

	~texture( void );

private:
	friend class context;

	texture( void );
	GLuint _texture;
};

////////////////////////////////////////

template<texture::target T>
class bound_texture
{
public:
	bound_texture( void ) = delete;
	bound_texture( const bound_texture &other ) = delete;
	bound_texture( bound_texture &&other )
	{
	}

	void image_2d( format f, size_t w, size_t h, image_type type, const void *data )
	{
		glTexImage2D( static_cast<GLenum>( T ), 0, static_cast<GLint>(f), w, h, 0, static_cast<GLenum>(f), static_cast<GLenum>(type), data );
	}
	
	void set_wrapping( wrapping w )
	{
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_WRAP_S, static_cast<GLint>(w) );
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_WRAP_T, static_cast<GLint>(w) );
	}

	void set_wrapping( wrapping s, wrapping t )
	{
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_WRAP_S, static_cast<GLint>(s) );
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_WRAP_T, static_cast<GLint>(t) );
	}

	void set_filters( filter min, filter mag )
	{
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min) );
		glTexParameteri( static_cast<GLenum>( T ), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag) );
	}

	void mipmap( void )
	{
		glGenerateMipmap( static_cast<GLenum>( T ) );
	}

private:
	friend class context;

	bound_texture( GLuint txt )
	{
		glBindTexture( static_cast<GLenum>( T ), txt );
	}
};

////////////////////////////////////////

}

