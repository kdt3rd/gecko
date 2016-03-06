
#include "framebuffer.h"

namespace gl
{

////////////////////////////////////////

framebuffer::framebuffer( void )
{
	glGenFramebuffers( 1, &_fb );
}

////////////////////////////////////////

framebuffer::framebuffer( GLuint fb )
	: _fb( fb )
{
}

////////////////////////////////////////

framebuffer::framebuffer( framebuffer &&other )
	: _fb( other._fb )
{
	other._fb = 0;
}

////////////////////////////////////////

framebuffer::~framebuffer( void )
{
	if ( _fb > 0 )
		glDeleteFramebuffers( 1, &_fb );
}

////////////////////////////////////////

}

