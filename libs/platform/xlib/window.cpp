
#include "window.h"
#include <base/pointer.h>

#include <string.h>
#include <iostream>

#include <base/contract.h>
#include <base/scope_guard.h>
#include <stdexcept>

#include <gl/opengl.h>
#include <gl/check.h>
#include <GL/glx.h>

namespace {

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static int visual_attribs[] =
{
//	GLX_X_RENDERABLE    , True,
	GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
	GLX_RENDER_TYPE     , GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
	GLX_RED_SIZE        , 8,
	GLX_GREEN_SIZE      , 8,
	GLX_BLUE_SIZE       , 8,
	GLX_ALPHA_SIZE      , 8,
	GLX_DEPTH_SIZE      , 24,
	GLX_STENCIL_SIZE    , 8,
	GLX_DOUBLEBUFFER    , True,
	GLX_SAMPLE_BUFFERS  , 1,
	GLX_SAMPLES         , 4,
	None
};

/*
static bool isExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  // Extension names should not have spaces.
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  // It takes a bit of care to be fool-proof about parsing the
  // OpenGL extensions string. Don't be fooled by sub-strings, etc.
  for (start=extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;

    start = terminator;
  }

  return false;
}
*/

}

namespace xlib
{

////////////////////////////////////////

window::window( const std::shared_ptr<Display> &dpy )
	: _display( dpy )
{
	precondition( _display, "null display" );

	Display *disp = _display.get();

	glXCreateContextAttribsARB = (GLXContext(*)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list))glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	glXChooseFBConfig = (GLXFBConfig*(*)(Display *dpy, int screen, const int *attrib_list, int *nelements))glXGetProcAddressARB((GLubyte*)"glXChooseFBConfig");
	glXGetVisualFromFBConfig = (XVisualInfo*(*)(Display *dpy, GLXFBConfig config))glXGetProcAddressARB((GLubyte*)"glXGetVisualFromFBConfig");
	glXGetFBConfigAttrib = (int(*)(Display *dpy, GLXFBConfig config, int attribute, int *value))glXGetProcAddressARB((GLubyte*)"glXGetFBConfigAttrib");

	// Check GLX version.  Version 1.3 is needed for FBConfig
	int glx_major, glx_minor;
	if ( !glXQueryVersion( disp, &glx_major, &glx_minor ) )
		throw std::runtime_error( "glx query version failed" );
	if ( ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
		throw std::runtime_error( "glx too old" );

	// Get the framebuffer configs
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig( disp, DefaultScreen( disp ), visual_attribs, &fbcount );
	if ( fbc == nullptr )
		throw std::runtime_error( "failed to get GL framebuffer configs" );
	on_scope_exit += [&]() { XFree( fbc ); };

	// Find the best framebuffer
	int best_fbc = -1, best_num_samp = -1;
	for ( int i = 0; i < fbcount; ++i )
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( disp, fbc[i] );
		on_scope_exit += [&]() { XFree( vi ); };

		if ( vi != nullptr )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib( disp, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( disp, fbc[i], GLX_SAMPLES, &samples );
			if ( best_fbc < 0 || ( samp_buf && samples > best_num_samp ) )
				best_fbc = i, best_num_samp = samples;
		}
	}

	GLXFBConfig bestFbc = fbc[ best_fbc ];

	XVisualInfo *vi = glXGetVisualFromFBConfig( disp, bestFbc );
	if ( vi == nullptr )
		throw std::runtime_error( "no glx visual found" );
	on_scope_exit += [&]() { XFree( vi ); };

	XSetWindowAttributes swa;
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask =
		ExposureMask | StructureNotifyMask | VisibilityChangeMask |
		EnterWindowMask | LeaveWindowMask |
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask | ButtonMotionMask;

	Window root = DefaultRootWindow( disp );
	swa.colormap = XCreateColormap( disp, root, vi->visual, AllocNone );

	_win = XCreateWindow( disp, root, 0, 0, 320, 240, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );

	// Get the default screen's GLX extension list
//	const char *glxExts = glXQueryExtensionsString( disp, DefaultScreen( disp ) );

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	auto glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

//	GLXContext tmp = glXCreateNewContext( disp, bestFbc, GLX_RGBA_TYPE, 0, True );
//	glXMakeCurrent( disp, _win, tmp );
	// Check for the GLX_ARB_create_context extension string and the function.
//	if ( glxewIsSupported( "GLX_ARB_create_context" ) == 1 )
	{
		// If it does, try to get a GL 4.0 context!
		int atrributes[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};

		_glc = glXCreateContextAttribsARB( disp, bestFbc, 0, True, atrributes );

		glXMakeCurrent( disp, _win, _glc );
//		glXDestroyContext( disp, tmp );
	}
//	else
//		throw std::runtime_error( "opengl 4 not supported" );

	checkgl();
	glewExperimental = true;
	GLenum err = glewInit();
	checkgl();
	if ( err != GLEW_OK )
		throw std::runtime_error( reinterpret_cast<const char *>( glewGetErrorString( err ) ) );

	_canvas = std::make_shared<draw::canvas>();

	// Sync to ensure any errors generated are processed.
	XSync( disp, False );
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
	XRaiseWindow( _display.get(), _win );
}

////////////////////////////////////////

void window::lower( void )
{
	XLowerWindow( _display.get(), _win );
}

////////////////////////////////////////

void window::set_popup( void )
{
	XSetWindowAttributes swa;
	swa.override_redirect = true;
	XChangeWindowAttributes( _display.get(), _win, CWOverrideRedirect, &swa );
}

////////////////////////////////////////

void window::show( void )
{
	XMapWindow( _display.get(), _win );
}

////////////////////////////////////////

void window::hide( void )
{
	XUnmapWindow( _display.get(), _win );
}

////////////////////////////////////////

bool window::is_visible( void )
{
	// TODO fix this
	return true;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::resize( double w, double h )
{
	XResizeWindow( _display.get(), _win, (unsigned int)( std::max( 0.0, w ) + 0.5 ), (unsigned int)( std::max( 0.0, h ) + 0.5 ) );
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	XStoreName( _display.get(), _win, t.c_str() );
}

////////////////////////////////////////

void window::invalidate( const base::rect &r )
{
	if ( !_invalid )
	{
		XClearArea( _display.get(), _win, 0, 0, 0, 0, True );
		_invalid = true;
	}
}

////////////////////////////////////////

gl::context window::context( void )
{
	glXMakeCurrent( _display.get(), _win, _glc );
	return gl::context();
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	glXMakeCurrent( _display.get(), _win, _glc );
	return _canvas;
}

////////////////////////////////////////

Window window::id( void ) const
{
	return _win;
}

////////////////////////////////////////

void window::move_event( double x, double y )
{
	if ( _last_x != x && _last_y != y )
	{
		_last_x = x;
		_last_y = y;
		moved( x, y );
	}
}

////////////////////////////////////////

void window::resize_event( double w, double h )
{
	if ( _last_w != h && _last_w != h )
	{
		_last_w = w;
		_last_h = h;
		glXMakeCurrent( _display.get(), _win, _glc );
		glViewport( 0, 0, w, h );
		resized( w, h );
	}
}

////////////////////////////////////////

void window::expose_event( void )
{
	_invalid = false;
	glXMakeCurrent( _display.get(), _win, _glc );
	exposed();
	glXSwapBuffers( _display.get(), _win );
	glFlush();
	XFlush( _display.get() );
}

////////////////////////////////////////

}

