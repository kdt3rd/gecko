//
// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "api.h"
#include <map>

namespace gl
{

////////////////////////////////////////

api::api( void )
{
	_matrix.emplace_back();
}

////////////////////////////////////////

api::~api( void )
{
}

////////////////////////////////////////

std::string api::get_vendor( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_VENDOR ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_VENDOR ) error" );
	return str;
}

////////////////////////////////////////

std::string api::get_renderer( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_RENDERER ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_RENDERER ) error" );
	return str;
}

////////////////////////////////////////

std::string api::get_version( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_VERSION ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_VENDOR ) error" );
	return str;
}

////////////////////////////////////////

std::string api::get_shading_version( void )
{
	auto str = reinterpret_cast<const char *>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	if ( str == nullptr )
		throw_runtime( "glGetString( GL_SHADING_LANGUAGE_VERSION ) error" );
	return str;
}

////////////////////////////////////////

size_t api::get_max_uniform_buffer_bindings( void )
{
	GLint binds;
	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &binds );
	return static_cast<size_t>( binds );
}

////////////////////////////////////////

void api::enable( capability cap )
{
	glEnable( static_cast<GLenum>( cap ) );
}

////////////////////////////////////////

void api::disable( capability cap )
{
	glDisable( static_cast<GLenum>( cap ) );
}

////////////////////////////////////////

void api::clear_color( const color &c )
{
	glClearColor( c.red(), c.green(), c.blue(), c.alpha() );
}

////////////////////////////////////////

void api::clear( buffer_bit bit )
{
	clear( static_cast<buffer_bits>( bit ) );
}

////////////////////////////////////////

void api::clear( buffer_bits bits )
{
	glClear( static_cast<GLbitfield>( bits ) );
}

////////////////////////////////////////

void api::depth_mask( bool write )
{
	glDepthMask( write );
}

////////////////////////////////////////

void api::stencil_mask( bool write )
{
	glStencilMask( write ? GLuint(~0) : GLuint(0) );
}

////////////////////////////////////////

void api::stencil_mask( uint32_t mask )
{
	glStencilMask( static_cast<GLuint>( mask ) );
}

////////////////////////////////////////

void api::color_mask( bool r, bool g, bool b, bool a )
{
	glColorMask( r, g, b, a );
}

////////////////////////////////////////

void api::blend_func( blend_style src, blend_style dst )
{
	glBlendFunc( static_cast<GLenum>( src ), static_cast<GLenum>( dst ) );
}

////////////////////////////////////////

void api::save_matrix( void )
{
	_matrix.emplace_back( _matrix.back() );
}

////////////////////////////////////////

void api::depth_func( depth_test t )
{
	glDepthFunc( static_cast<GLenum>( t ) );
}

////////////////////////////////////////

void api::ortho( float left, float right, float top, float bottom )
{
	multiply( gl::matrix4::ortho( left, right, top, bottom ) );
}

////////////////////////////////////////

void api::scale( float x, float y, float z )
{
	multiply( gl::matrix4::scaling( x, y, z ) );
}

////////////////////////////////////////

void api::translate( float dx, float dy, float dz )
{
	multiply( gl::matrix4::translation( dx, dy, dz ) );
}

////////////////////////////////////////

void api::multiply( const matrix4 &m )
{
	_matrix.back() *= m;
}

////////////////////////////////////////

void api::restore_matrix( void )
{
	precondition( _matrix.size() > 1, "too many restore_matrix" );
	_matrix.pop_back();
}

////////////////////////////////////////

void api::reset( void )
{
	_matrix.clear();
	_matrix.emplace_back();
	_view = gl::matrix4::identity();
	_projection = gl::matrix4::identity();
}

////////////////////////////////////////

#ifndef __APPLE__

namespace
{
std::map<GLenum,const char *> glsource {
	{ GL_DEBUG_SOURCE_API, "API" },
	{ GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM" },
	{ GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER" },
	{ GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY" },
	{ GL_DEBUG_SOURCE_APPLICATION, "APPLICATION" },
	{ GL_DEBUG_SOURCE_OTHER, "OTHER" }
};

std::map<GLenum,const char *> gltype {
	{ GL_DEBUG_TYPE_ERROR, "ERROR" },
	{ GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR" },
	{ GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR" },
	{ GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY" },
	{ GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE" },
	{ GL_DEBUG_TYPE_OTHER, "OTHER" },
	{ GL_DEBUG_TYPE_MARKER, "MARKER" },
	{ GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP" },
	{ GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP" }
};

std::map<GLenum,const char *> glseverity {
	{ GL_DEBUG_SEVERITY_LOW, "LOW" },
	{ GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM" },
	{ GL_DEBUG_SEVERITY_HIGH, "HIGH" },
	{ GL_DEBUG_SEVERITY_NOTIFICATION, "LOW" }
};

void gldebugging( GLenum source, GLenum type, GLuint, GLenum severity, GLsizei length, const GLchar *message, const void * )
{
	std::clog << "Source:   " << glsource[source] << '\n';
	std::clog << "Type:     " << gltype[type] << '\n';
	std::clog << "Severity: " << glseverity[severity] << '\n';
	std::clog << "Message:  " << std::string( message, static_cast<size_t>( length ) ) << '\n';
	std::clog << std::endl;
};

}

#endif

void api::setup_debugging( void )
{
#ifndef __APPLE__
	GLint flags;
	glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
	if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		glDebugMessageCallback( &gldebugging, nullptr );
#endif
}

////////////////////////////////////////

}
