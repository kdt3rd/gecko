// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "context.h"

#include <base/contract.h>
#include <gl/opengl.h>

////////////////////////////////////////

namespace platform
{
namespace wayland
{
////////////////////////////////////////

context::context( EGLDisplay disp ) : _disp( disp )
{
#if __cplusplus > 201402L
    _api = std::make_unique<gl::api>();
#else
    _api.reset( new gl::api );
#endif
}

////////////////////////////////////////

context::~context( void )
{
    eglDestroySurface( _disp, _surface );
    eglDestroyContext( _disp, _ctxt );
}

////////////////////////////////////////

void context::create( EGLNativeWindowType nwin )
{
    EGLint attributes[]  = { EGL_RED_SIZE,
                            8,
                            EGL_GREEN_SIZE,
                            8,
                            EGL_BLUE_SIZE,
                            8,
                            EGL_DEPTH_SIZE,
                            24,
                            EGL_CONFORMANT,
                            EGL_OPENGL_BIT,
                            EGL_RENDERABLE_TYPE,
                            EGL_OPENGL_BIT,
                            EGL_NONE,
                            EGL_NONE };
    EGLint ctxtattribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        3,
        EGL_CONTEXT_MINOR_VERSION,
        3,
        //		EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
        //		EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_TRUE,
        EGL_NONE,
        EGL_NONE
    };
    EGLConfig config;
    EGLint    num_config = 0;

    if ( !eglChooseConfig( _disp, attributes, &config, 1, &num_config ) )
        throw std::runtime_error( "unable to find valid egl context config" );
    if ( 0 == num_config )
        throw std::runtime_error( "unable to find valid egl context config" );

    TODO( "Implement shared context - create one in system, pass it here" )
    _ctxt = eglCreateContext( _disp, config, EGL_NO_CONTEXT, ctxtattribs );
    if ( !_ctxt )
        throw std::runtime_error( "Unable to create egl context" );

    _surface = eglCreateWindowSurface( _disp, config, nwin, nullptr );

    acquire();
}

////////////////////////////////////////

void context::share( const ::base::context &o )
{
    ::platform::context::share( o );

    throw_not_yet();
}

////////////////////////////////////////

context::render_query context::render_query_func( void )
{
    return reinterpret_cast<render_query>( eglGetProcAddress );
}

////////////////////////////////////////

void context::set_viewport(
    coord_type x, coord_type y, coord_type w, coord_type h )
{}

////////////////////////////////////////

void context::swap_buffers( void ) { eglSwapBuffers( _disp, _surface ); }

////////////////////////////////////////

void context::acquire( void )
{
    eglMakeCurrent( _disp, _surface, _surface, _ctxt );
}

////////////////////////////////////////

void context::release( void ) { eglMakeCurrent( _disp, nullptr, nullptr, nullptr ); }

////////////////////////////////////////

void context::reset_clip( const rect &r ) {}

////////////////////////////////////////

} // namespace wayland
} // namespace platform
