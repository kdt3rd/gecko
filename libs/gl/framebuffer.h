
#pragma once

#include "texture.h"

namespace gl
{

class framebuffer;

////////////////////////////////////////

class bound_framebuffer
{
public:
	bound_framebuffer( void )
	{
	}

	bound_framebuffer( const bound_framebuffer &other ) = delete;

	bound_framebuffer( bound_framebuffer &&/*other*/ )
	{
	}

	void attach( const texture &t )
	{
		std::cout << "TEXTURE ID: " << t.id() << std::endl;
		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.id(), 0 );
	}

private:
	friend class framebuffer;

	bound_framebuffer( GLuint fb )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, fb );
	}
};

////////////////////////////////////////

class framebuffer
{
public:
	framebuffer( void );

	framebuffer( GLuint fb );

	framebuffer( framebuffer &&other );

	~framebuffer( void );

	inline bool is_valid( void ) const
	{
		return _fb > 0;
	}

	bound_framebuffer bind( void )
	{
		return bound_framebuffer( _fb );
	}

private:
	GLuint _fb = 0;
};

////////////////////////////////////////

}

