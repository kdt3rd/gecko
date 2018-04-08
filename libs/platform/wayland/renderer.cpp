//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "renderer.h"

#include <mutex>
#include <dlfcn.h>
#include <stdexcept>

#include <EGL/egl.h>

////////////////////////////////////////

namespace platform
{
namespace wayland
{

////////////////////////////////////////

renderer::renderer( void )
{
}

////////////////////////////////////////

renderer::~renderer( void )
{
}

////////////////////////////////////////

std::shared_ptr<context>
renderer::create_context( const std::shared_ptr<::platform::window> &w )
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
	return reinterpret_cast<render_query>( eglGetProcAddress );
}


////////////////////////////////////////

} // namespace wayland
} // namespace platform



