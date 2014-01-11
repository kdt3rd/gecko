
#include "context.h"

namespace gl
{

////////////////////////////////////////

context::context( void )
{
}

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

void context::enable( capability cap )
{
	glEnable( static_cast<GLenum>( cap ) );
}

////////////////////////////////////////

void context::disable( capability cap )
{
	glDisable( static_cast<GLenum>( cap ) );
}

////////////////////////////////////////

void context::clear_color( const draw::color &c )
{
	glClearColor( c.red(), c.green(), c.blue(), c.alpha() );
}

////////////////////////////////////////

void context::clear( buffer_bit bit )
{
	clear( static_cast<buffer_bits>( bit ) );
}

////////////////////////////////////////

void context::clear( buffer_bits bits )
{
	glClear( bits );
}

////////////////////////////////////////

void context::depth_mask( bool write )
{
	glDepthMask( write );
}

////////////////////////////////////////

void context::stencil_mask( bool write )
{
	glStencilMask( write ? ~0 : 0 );
}

////////////////////////////////////////

void context::stencil_mask( uint32_t mask )
{
	glStencilMask( mask );
}

////////////////////////////////////////

void context::color_mask( bool r, bool g, bool b, bool a )
{
	glColorMask( r, g, b, a );
}

////////////////////////////////////////

}

