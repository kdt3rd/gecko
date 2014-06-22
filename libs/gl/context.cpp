
#include "context.h"

namespace gl
{

////////////////////////////////////////

context::context( void )
{
	_matrix.emplace_back();
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

void context::clear_color( const core::color &c )
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

void context::blend_func( blend_style src, blend_style dst )
{
	glBlendFunc( static_cast<GLenum>( src ), static_cast<GLenum>( dst ) );
}

////////////////////////////////////////

void context::save_matrix( void )
{
	_matrix.emplace_back( _matrix.back() );
}

////////////////////////////////////////

void context::ortho( float left, float right, float top, float bottom )
{
	multiply( gl::matrix4::ortho( left, right, top, bottom ) );
}

////////////////////////////////////////

void context::scale( float x, float y, float z )
{
	multiply( gl::matrix4::scale( x, y, z ) );
}

////////////////////////////////////////

void context::translate( float dx, float dy, float dz )
{
	multiply( gl::matrix4::translation( dx, dy, dz ) );
}

////////////////////////////////////////

void context::multiply( const matrix4 &m )
{
	_matrix.back() *= m;
}

////////////////////////////////////////

void context::restore_matrix( void )
{
	precondition( _matrix.size() > 1, "too many restore_matrix" );
	_matrix.pop_back();
}

////////////////////////////////////////

}

