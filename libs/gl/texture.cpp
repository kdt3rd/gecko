
#include "texture.h"

namespace gl
{

////////////////////////////////////////

texture::texture( void )
{
	glGenTextures( 1, &_texture );
}

////////////////////////////////////////

texture::texture( GLuint t )
	: _texture( t )
{
}

////////////////////////////////////////

texture::texture( texture &&other )
	: _texture( other._texture )
{
	other._texture = 0;
}

////////////////////////////////////////

texture::~texture( void )
{
	if ( _texture > 0 )
		glDeleteTextures( 1, &_texture );
}

////////////////////////////////////////

}

