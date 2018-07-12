//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "context.h"

#include <base/contract.h>
#include <base/scope_guard.h>

#include <dlfcn.h>
#include <stdexcept>

#include <gl/opengl.h>

namespace {

const int visual_attribs[] =
{
//  GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 2, // only use 2 in case we're on a 10-bit display
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    GLX_SAMPLE_BUFFERS  , 1,
    GLX_SAMPLES         , 4,
    None
};

void *opengl_dso = nullptr;
platform::context::render_query glx_query = nullptr;

void shutdown_opengl( void )
{
    if ( opengl_dso )
        dlclose( opengl_dso );
    glx_query = nullptr;
}

platform::context::render_func_ptr
queryGL( const char *fname )
{
    platform::context::render_func_ptr ret = nullptr;
    if ( glx_query )
        ret = glx_query( fname );
    if ( ! ret && opengl_dso )
        ret = (platform::context::render_func_ptr) dlsym( opengl_dso, fname );

    return ret;
}

void (*_glc_makecurrent)( Display *, GLXDrawable, GLXContext ) = nullptr;
void (*_glc_swapbuffers)( Display *, GLXDrawable ) = nullptr;
void (*_glc_scissor)( GLint, GLint, GLsizei, GLsizei ) = nullptr;
void (*_glc_viewport)( GLint, GLint, GLsizei, GLsizei ) = nullptr;
void (*_glc_enable)( GLenum ) = nullptr;
void (*_glc_disable)( GLenum ) = nullptr;

Bool (*_glx_queryversion)( Display *, int *, int * ) = nullptr;
PFNGLXCHOOSEFBCONFIGPROC _glx_choosefbconfig = nullptr;
PFNGLXGETVISUALFROMFBCONFIGPROC _glx_getvisual = nullptr;
PFNGLXGETFBCONFIGATTRIBPROC _glx_getfbattr = nullptr;
PFNGLXCREATECONTEXTATTRIBSARBPROC _glx_createctxt = nullptr;
void (*_glx_destroyctxt)( Display *, GLXContext ) = nullptr;
PFNGLXCREATEWINDOWPROC _glx_createwindow = nullptr;
PFNGLXDESTROYWINDOWPROC _glx_destroywindow = nullptr;

void init_opengl( void )
{
    opengl_dso = dlopen( "libGL.so", RTLD_GLOBAL | RTLD_LAZY );
    if ( opengl_dso )
    {
        glx_query = (platform::context::render_query) dlsym( opengl_dso, "glXGetProcAddressARB" );
        atexit( shutdown_opengl );
    }

    if ( ! gl3wInit2( queryGL ) )
        throw_runtime( "Unable to initialize OpenGL" );
}

void check_functions( Display *disp )
{
    if ( _glx_queryversion != nullptr )
        return;

    init_opengl();

    _glx_queryversion = reinterpret_cast<Bool (*)(Display *, int *, int *)>( queryGL( "glXQueryVersion" ) );
    if ( ! _glx_queryversion  )
        throw_runtime( "Unable to find glXQueryVersion" );

    // Check GLX version.  Version 1.3 is needed for FBConfig
    int glx_major, glx_minor;
    if ( !_glx_queryversion( disp, &glx_major, &glx_minor ) )
        throw_runtime( "glx query version failed" );
    if ( ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
        throw_runtime( "glx too old, need 1.3 or newer" );

    // seems ok, let's keep going
    _glx_choosefbconfig = reinterpret_cast<PFNGLXCHOOSEFBCONFIGPROC>( queryGL( "glXChooseFBConfig" ) );
    if ( ! _glx_choosefbconfig )
        throw_runtime( "Unable to find glXChooseFBConfig" );
    _glx_getvisual = reinterpret_cast<PFNGLXGETVISUALFROMFBCONFIGPROC>( queryGL( "glXGetVisualFromFBConfig" ) );
    if ( ! _glx_getvisual )
        throw_runtime( "Unable to find glXGetVisualFromFBConfig" );

    _glx_getfbattr = reinterpret_cast<PFNGLXGETFBCONFIGATTRIBPROC>( queryGL( "glXGetFBConfigAttrib" ) );
    if ( ! _glx_getfbattr )
        throw_runtime( "Unable to find glXGetFBConfigAttrib" );

    _glx_createctxt = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>( queryGL( "glXCreateContextAttribsARB" ) );
    if ( ! _glx_createctxt )
        throw_runtime( "unable to find glXCreateContextAttribsARB" );

    _glx_destroyctxt = reinterpret_cast<void (*)(Display *, GLXContext)>( queryGL( "glXDestroyContext" ) );
    if ( ! _glx_destroyctxt )
        throw_runtime( "unable to find glXDestroyContext" );

    _glx_createwindow = reinterpret_cast<PFNGLXCREATEWINDOWPROC>( queryGL( "glXCreateWindow" ) );
    if ( ! _glx_createwindow )
        throw_runtime( "GLX 1.3 or newer, but no glXCreateWindow" );

    _glx_destroywindow = reinterpret_cast<PFNGLXDESTROYWINDOWPROC>( queryGL( "glXDestroyWindow" ) );
    if ( ! _glx_destroywindow )
        throw_runtime( "GLX 1.3 or newer, but no glXDestroyWindow" );

    _glc_makecurrent = reinterpret_cast<void (*)(Display *, GLXDrawable, GLXContext )>( queryGL( "glXMakeCurrent" ) );
    if ( ! _glc_makecurrent )
        throw_runtime( "unable to retrieve glXMakeCurrent" );

    _glc_swapbuffers = reinterpret_cast<void (*)(Display *, GLXDrawable )>( queryGL( "glXSwapBuffers" ) );
    if ( ! _glc_swapbuffers )
        throw_runtime( "unable to retrieve glXSwapBuffers" );
    _glc_scissor = reinterpret_cast<void (*)( GLint, GLint, GLsizei, GLsizei )>( queryGL( "glScissor" ) );
    if ( ! _glc_scissor )
        throw_runtime( "unable to retrieve glScissor" );
    _glc_viewport = reinterpret_cast<void (*)( GLint, GLint, GLsizei, GLsizei )>( queryGL( "glViewport" ) );
    if ( ! _glc_viewport )
        throw_runtime( "unable to retrieve glViewport" );
    _glc_enable = reinterpret_cast<void (*)( GLenum )>( queryGL( "glEnable" ) );
    if ( ! _glc_enable )
        throw_runtime( "unable to retrieve glEnable" );
    _glc_disable = reinterpret_cast<void (*)( GLenum )>( queryGL( "glDisable" ) );
    if ( ! _glc_disable )
        throw_runtime( "unable to retrieve glDisable" );
}

} // empty namespace

////////////////////////////////////////

namespace platform
{
namespace xlib
{

////////////////////////////////////////

context::context( const std::shared_ptr<Display> &dpy )
    : _display( dpy )
{
    check_functions( _display.get() );

#if __cplusplus > 201402L
    _api = std::make_unique<gl::api>();
#else
    _api.reset( new gl::api );
#endif
}

////////////////////////////////////////

context::~context( void )
{
    _glx_destroywindow( _display.get(), _win );
    _glx_destroyctxt( _display.get(), _ctxt );
}

////////////////////////////////////////

XVisualInfo *context::choose_best_config( void )
{
    // should we check all of them?
    precondition( _glx_createctxt, "GL properly initialized" );

    Display *disp = _display.get();

    // Get the framebuffer configs
    int fbcount;
    GLXFBConfig* fbc = _glx_choosefbconfig( disp, DefaultScreen( disp ), visual_attribs, &fbcount );
    if ( fbc == nullptr )
        throw_runtime( "failed to get GL framebuffer configs" );
    on_scope_exit { XFree( fbc ); };

    // Find the best framebuffer
    int best_fbc = -1, best_num_samp = -1;
    for ( int i = 0; i < fbcount; ++i )
    {
        XVisualInfo *vi = _glx_getvisual( disp, fbc[i] );
        on_scope_exit { XFree( vi ); };

        if ( vi != nullptr )
        {
            int samp_buf, samples;
            _glx_getfbattr( disp, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            _glx_getfbattr( disp, fbc[i], GLX_SAMPLES, &samples );
            if ( best_fbc < 0 || ( samp_buf && samples > best_num_samp ) )
                best_fbc = i, best_num_samp = samples;
        }
    }

    postcondition( best_fbc >= 0, "expect at least one frame buffer config" );
    _bestFBC = fbc[ best_fbc ];
    return _glx_getvisual( disp, _bestFBC );
}

////////////////////////////////////////

void context::create( Window win )
{
    precondition( _bestFBC != None, "choose visual successful" );

    int attributes[] =
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB|GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        //GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        //GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        None
    };

    Display *disp = _display.get();

    _ctxt = _glx_createctxt( disp, _bestFBC, 0, True, attributes );
    if ( ! _ctxt )
        throw std::runtime_error( "Unable to create OpenGL context" );

    _win = _glx_createwindow( disp, _bestFBC, win, nullptr );
    if ( _win == None )
        throw_runtime( "Unable to create GLX drawable" );

    acquire();
    if ( !gl3wIsSupported( 3, 3 ) )
        throw std::runtime_error( "opengl 3.3 not supported" );
}

////////////////////////////////////////

context::render_query context::render_query_func( void )
{
    return queryGL;
}

////////////////////////////////////////

void context::share( ::platform::context &o )
{
    throw_not_yet();
}

////////////////////////////////////////

void context::set_viewport( coord_type x, coord_type y, coord_type w, coord_type h )
{
    _glc_viewport( static_cast<GLint>(x), static_cast<GLint>(y),
                   static_cast<GLsizei>(w), static_cast<GLsizei>(h) );
}

////////////////////////////////////////

void context::swap_buffers( void )
{
    _glc_swapbuffers( _display.get(), _win );
}

////////////////////////////////////////

void context::acquire( void )
{
    _glc_makecurrent( _display.get(), _win, _ctxt );
}

////////////////////////////////////////

void context::release( void )
{
    _glc_makecurrent( _display.get(), None, nullptr );
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
                      static_cast<GLint>( r.y() ),
                      static_cast<GLsizei>( r.width() ),
                      static_cast<GLsizei>( r.height() ) );
    }
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform
