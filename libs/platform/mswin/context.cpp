// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "context.h"

#include <base/contract.h>
#include <mutex>

////////////////////////////////////////

namespace
{
HGLRC
WINAPI ( *wglCreateContextAttribsARB )( HDC, HGLRC, const int * ) = nullptr;

// prescribed method to enable nvidia GPU on laptops w/ dual GPU
//extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

HMODULE libgl       = nullptr;
HWND    coreWindow  = nullptr;
HGLRC   coreContext = nullptr;

std::once_flag opengl_init_flag;

platform::mswin::context::render_func_ptr queryGL( const char *f )
{
    //	std::call_once( opengl_init_flag, [](){ init_libgl(); } );
    using func_ptr = platform::mswin::context::render_func_ptr;
    func_ptr res   = nullptr;

    HGLRC oldCtxt = wglGetCurrentContext();
    HDC   oldDC   = wglGetCurrentDC();
    if ( oldCtxt != coreContext )
        wglMakeCurrent( GetDC( coreWindow ), coreContext );

    res = (func_ptr)wglGetProcAddress( f );

    if ( oldCtxt != coreContext )
        wglMakeCurrent( oldDC, oldCtxt );

    if ( !res && libgl )
        res = (func_ptr)GetProcAddress( libgl, f );

    return res;
}

HGLRC doCreate( HDC dc, HGLRC parContext )
{
    bool docreate = dc != nullptr;

    if ( dc == nullptr )
    {
        if ( coreWindow == nullptr )
        {
            coreWindow = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "WindowClass",
                "Title",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                nullptr,
                nullptr,
                GetModuleHandle( nullptr ),
                nullptr );
            if ( coreWindow == nullptr )
                throw_runtime( "Unable to create window" );
        }

        dc = GetDC( coreWindow );
    }

    // Create a new PIXELFORMATDESCRIPTOR (PFD)
    PIXELFORMATDESCRIPTOR pfd;
    memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
    pfd.nSize      = sizeof( PIXELFORMATDESCRIPTOR );
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24; //32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // Check if our PFD is valid and get a pixel format back
    int nPixelFormat = ChoosePixelFormat( dc, &pfd );
    if ( nPixelFormat == 0 )
        throw std::runtime_error( "pixel format failed" );

    // Try and set the pixel format based on our PFD
    bool bResult = SetPixelFormat( dc, nPixelFormat, &pfd );
    if ( !bResult )
        throw_runtime( "set pixel format failed" );

    int attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
        0
    };

    if ( !coreContext )
    {
        HGLRC oldCtxt = wglGetCurrentContext();

        HDC tmpdc = GetDC( coreWindow );

        HGLRC tempOpenGLContext = wglCreateContext( tmpdc );
        wglMakeCurrent( tmpdc, tempOpenGLContext );

        wglCreateContextAttribsARB =
            reinterpret_cast<decltype( wglCreateContextAttribsARB )>(
                wglGetProcAddress( "wglCreateContextAttribsARB" ) );
        if ( !wglCreateContextAttribsARB )
        {
            wglMakeCurrent( tmpdc, oldCtxt );
            wglDeleteContext( tempOpenGLContext );
            DestroyWindow( coreWindow );
            coreWindow = nullptr;
            throw_runtime( "Unable to find wglCreateContextAttribsARB" );
        }

        coreContext =
            wglCreateContextAttribsARB( tmpdc, parContext, attributes );
        if ( coreContext == nullptr )
        {
            wglMakeCurrent( dc, oldCtxt );
            wglDeleteContext( tempOpenGLContext );
            DestroyWindow( coreWindow );
            coreWindow = nullptr;
            throw_lasterror( "unable to create core OpenGL context" );
        }

        wglMakeCurrent( tmpdc, coreContext );
        wglDeleteContext( tempOpenGLContext );
        std::cout << "initializing OpenGL function pointers..." << std::endl;
        bool ok = gl3wInit2( queryGL );

        wglMakeCurrent( tmpdc, oldCtxt );
        if ( !ok )
            throw_runtime( "Unable to initialize OpenGL" );
        std::cout << "finished initializing OpenGL..." << std::endl;
    }

    if ( docreate )
    {
        HGLRC oldCtxt = wglGetCurrentContext();
        wglMakeCurrent( dc, coreContext );
        HGLRC ret = wglCreateContextAttribsARB( dc, parContext, attributes );
        wglMakeCurrent( dc, oldCtxt );
        if ( ret == nullptr )
            throw_lasterror( "unable to create OpenGL context" );
        return ret;
    }
    return nullptr;
}

void shutdown_libgl( void )
{
    wglMakeCurrent( nullptr, nullptr );
    if ( coreContext )
    {
        wglDeleteContext( coreContext );
        coreContext = nullptr;
    }
    if ( coreWindow )
    {
        DestroyWindow( coreWindow );
        coreWindow = nullptr;
    }
    if ( libgl )
        FreeLibrary( libgl );
    libgl = nullptr;
}

void init_libgl( void )
{
    libgl = LoadLibraryA( "opengl32.dll" );
    atexit( shutdown_libgl );

    doCreate( nullptr, nullptr );
}

} // anonymous namespace

////////////////////////////////////////

namespace platform
{
namespace mswin
{
////////////////////////////////////////

context::context( void )
{
    std::call_once( opengl_init_flag, []() { init_libgl(); } );

#if __cplusplus > 201402L
    _api = std::make_unique<gl::api>();
#else
    _api.reset( new gl::api );
#endif
}

////////////////////////////////////////

context::~context( void )
{
    if ( _hrc )
    {
        HGLRC oldCtxt = wglGetCurrentContext();
        if ( oldCtxt == _hrc )
            wglMakeCurrent( _hdc, nullptr );
        wglDeleteContext( _hrc );
    }
}

////////////////////////////////////////

context::render_query context::render_query_func( void ) { return queryGL; }

////////////////////////////////////////

void context::init( HWND hwnd )
{
    precondition( _hrc == nullptr, "expect uninitialized context" );
    _hdc = GetDC( hwnd );
    _hrc = doCreate( _hdc, coreContext );

    wglMakeCurrent( _hdc, _hrc );

    if ( !gl3wIsSupported( 3, 3 ) )
        throw_runtime( "OpenGL 3.3 not supported" );
}

////////////////////////////////////////

void context::share( const ::base::context &o ) {}

////////////////////////////////////////

void context::set_viewport(
    coord_type x, coord_type y, coord_type w, coord_type h )
{
    glViewport(
        static_cast<GLint>( x ),
        static_cast<GLint>( y ),
        static_cast<GLsizei>( w ),
        static_cast<GLsizei>( h ) );
    _last_vp[0] = x;
    _last_vp[1] = y;
    _last_vp[2] = w;
    _last_vp[3] = h;
}

////////////////////////////////////////

void context::swap_buffers( void ) { SwapBuffers( _hdc ); }

////////////////////////////////////////

void context::acquire( void )
{
    //	PAINTSTRUCT ps;
    //	BeginPaint( _hwnd, &ps );
    if ( !wglMakeCurrent( _hdc, _hrc ) )
        throw std::runtime_error( "couldn't make context current" );
}

////////////////////////////////////////

void context::release( void ) { wglMakeCurrent( nullptr, nullptr ); }

////////////////////////////////////////

void context::reset_clip( const rect &r )
{
    glEnable( GL_SCISSOR_TEST );
    glScissor(
        static_cast<GLint>( r.x() ),
        static_cast<GLint>( _last_vp[3] - ( r.y() + r.height() ) ),
        static_cast<GLsizei>( r.width() ),
        static_cast<GLsizei>( r.height() ) );
}

} // namespace mswin
} // namespace platform
