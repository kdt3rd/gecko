//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "context.h"

#include <gl/gl3w.h>

#include <mutex>
#include <stdexcept>
#include <dlfcn.h>

#include <Cocoa/Cocoa.h>

namespace
{
void *libgl = nullptr;
std::once_flag opengl_init_flag;

platform::context::render_func_ptr
queryGL( const char *f )
{
	if ( libgl )
		return reinterpret_cast<platform::context::render_func_ptr>( dlsym( libgl, f ) );

	return nullptr;
}

void (*_glc_scissor)( GLint, GLint, GLsizei, GLsizei ) = nullptr;
void (*_glc_viewport)( GLint, GLint, GLsizei, GLsizei ) = nullptr;
void (*_glc_enable)( GLenum ) = nullptr;
void (*_glc_disable)( GLenum ) = nullptr;

void shutdown_libgl(void)
{
	if ( libgl )
		dlclose( libgl );
    _glc_scissor = nullptr;
    _glc_viewport = nullptr;
    _glc_enable = nullptr;
    _glc_disable = nullptr;
}

void init_libgl(void)
{
	libgl = dlopen( "/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_GLOBAL | RTLD_LAZY );

	if ( ! gl3wInit2( queryGL ) )
		throw_runtime( "Unable to initialize OpenGL" );

	atexit( shutdown_libgl );
    _glc_scissor = reinterpret_cast<void (*)(GLint, GLint, GLsizei, GLsizei)>( queryGL( "glScissor" ) );
    _glc_viewport = reinterpret_cast<void (*)(GLint, GLint, GLsizei, GLsizei)>( queryGL( "glViewport" ) );
    _glc_enable = reinterpret_cast<void (*)(GLenum)>( queryGL( "glEnable" ) );
    _glc_disable = reinterpret_cast<void (*)(GLenum)>( queryGL( "glDisable" ) );

}

} // empty namespace

namespace platform { namespace cocoa
{

////////////////////////////////////////

context::context( void )
{
	std::call_once( opengl_init_flag, [](){ init_libgl(); } );

	_api.reset( new gl::api );
}

////////////////////////////////////////

void context::set_ns( void *w, void *v )
{
	_window = w;
	_view = v;
}

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

context::render_query
context::render_query_func( void )
{
	return queryGL;
}

////////////////////////////////////////

void context::share( ::platform::context & )
{
}

////////////////////////////////////////

void context::set_viewport( coord_type x, coord_type y, coord_type w, coord_type h )
{
    _glc_viewport( static_cast<GLint>(x), static_cast<GLint>(y),
                   static_cast<GLsizei>(w), static_cast<GLsizei>(h) );
    _last_vp[0] = x;
    _last_vp[1] = y;
    _last_vp[2] = w;
    _last_vp[3] = h;
}

////////////////////////////////////////

void context::swap_buffers( void )
{
}

////////////////////////////////////////

void context::acquire( void )
{
	NSOpenGLView *v = static_cast<NSOpenGLView *>( _view );
	[[v openGLContext] makeCurrentContext];
}

////////////////////////////////////////

void context::release( void )
{
	NSOpenGLView *v = static_cast<NSOpenGLView *>( _view );
	[[v openGLContext] flushBuffer];
}

////////////////////////////////////////

void context::reset_clip( const rect &r )
{
    if ( r.empty() )
    {
        _glc_disable( GL_SCISSOR_TEST );
    }
    else
    {
        _glc_enable( GL_SCISSOR_TEST );
        _glc_scissor( static_cast<GLint>( r.x() ),
                      static_cast<GLint>( _last_vp[3] - ( r.y() + r.height() ) ),
                      static_cast<GLsizei>( r.width() ),
                      static_cast<GLsizei>( r.height() ) );
    }
}

} // namespace cocoa
} // namespace platform
