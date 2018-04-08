//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "renderer.h"

#include <base/contract.h>

#include <mutex>
#include <dlfcn.h>
#include <stdexcept>

#include <gl/opengl.h>

namespace {

void *opengl_dso = nullptr;
platform::renderer::render_query glx_query = nullptr;
std::once_flag opengl_init_flag;

void shutdown_opengl( void )
{
	if ( opengl_dso )
		dlclose( opengl_dso );
	glx_query = nullptr;
}

void init_opengl( void )
{
	opengl_dso = dlopen( "libGL.so", RTLD_GLOBAL | RTLD_LAZY );
	if ( opengl_dso )
	{
		glx_query = (platform::renderer::render_query) dlsym( opengl_dso, "glXGetProcAddressARB" );
		atexit( shutdown_opengl );
	}
}

platform::renderer::render_func_ptr
queryGL( const char *fname )
{
	platform::renderer::render_func_ptr ret = nullptr;
	if ( glx_query )
		ret = glx_query( fname );
	if ( ! ret && opengl_dso )
		ret = (platform::renderer::render_func_ptr) dlsym( opengl_dso, fname );

	return ret;
}

} // empty namespace

////////////////////////////////////////

namespace platform
{
namespace xlib
{

////////////////////////////////////////

renderer::renderer( void )
{
	std::call_once( opengl_init_flag, [](){ init_opengl(); } );

	if ( ! gl3wInit2( queryGL ) )
		throw_runtime( "Unable to initialize OpenGL" );
}

////////////////////////////////////////

renderer::~renderer( void )
{
}

////////////////////////////////////////

std::shared_ptr<context> renderer::create_context( const std::shared_ptr<::platform::window> &w )
{
    return std::shared_ptr<context>();
}

////////////////////////////////////////

void renderer::activate( const std::shared_ptr<context> &c )
{
}

////////////////////////////////////////

void renderer::deactivate( const std::shared_ptr<context> &c )
{
}

////////////////////////////////////////

renderer::render_query renderer::render_query_func( void )
{
	return queryGL;
}


////////////////////////////////////////

} // namespace xlib
} // namespace platform



