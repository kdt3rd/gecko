
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

std::string context::get_vendor( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_VENDOR ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_VENDOR ) error" );
	return str;
}

////////////////////////////////////////

std::string context::get_renderer( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_RENDERER ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_RENDERER ) error" );
	return str;
}

////////////////////////////////////////

std::string context::get_version( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_VERSION ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_VENDOR ) error" );
	return str;
}

////////////////////////////////////////

std::string context::get_shading_version( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_SHADING_LANGUAGE_VERSION ) error" );
	return str;
}

////////////////////////////////////////

size_t context::get_max_uniform_buffer_bindings( void )
{
	GLint binds;
	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &binds );
	return static_cast<size_t>( binds );
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

void context::clear_color( const base::color &c )
{
	glClearColor( static_cast<GLfloat>( c.red() ), static_cast<GLfloat>( c.green() ),
				  static_cast<GLfloat>( c.blue() ), static_cast<GLfloat>( c.alpha() ) );
}

////////////////////////////////////////

void context::clear( buffer_bit bit )
{
	clear( static_cast<buffer_bits>( bit ) );
}

////////////////////////////////////////

void context::clear( buffer_bits bits )
{
	glClear( static_cast<GLbitfield>( bits ) );
}

////////////////////////////////////////

void context::depth_mask( bool write )
{
	glDepthMask( write );
}

////////////////////////////////////////

void context::stencil_mask( bool write )
{
	glStencilMask( write ? GLuint(~0) : GLuint(0) );
}

////////////////////////////////////////

void context::stencil_mask( uint32_t mask )
{
	glStencilMask( static_cast<GLuint>( mask ) );
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

void context::depth_func( depth_test t )
{
	glDepthFunc( static_cast<GLenum>( t ) );
}

////////////////////////////////////////

void context::viewport( int64_t x, int64_t y, size_t w, size_t h )
{
	glViewport( static_cast<GLint>( x ), static_cast<GLint>( y ), static_cast<GLsizei>( w ), static_cast<GLsizei>( h ) );
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

