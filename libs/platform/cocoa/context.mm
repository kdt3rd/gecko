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
		return (platform::context::render_func_ptr) dlsym( libgl, f );

	return nullptr;
}

void shutdown_libgl(void)
{
	if ( libgl )
		dlclose( libgl );
}

void init_libgl(void)
{
	libgl = dlopen( "/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_GLOBAL | RTLD_LAZY );

	if ( ! gl3wInit2( queryGL ) )
		throw_runtime( "Unable to initialize OpenGL" );

	atexit( shutdown_libgl );
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

void context::share( ::platform::context &o )
{
}

////////////////////////////////////////

void context::set_viewport( coord_type x, coord_type y, coord_type w, coord_type h )
{
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
}
	
} // namespace cocoa
} // namespace platform
