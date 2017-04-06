//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "framebuffer.h"

namespace gl
{

////////////////////////////////////////

std::vector<framebuffer::binding *> framebuffer::binding::_stack;

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

