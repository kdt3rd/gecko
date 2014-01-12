
#include "window.h"
#include <draw/cairo/canvas.h>
#include <core/pointer.h>

#include <string.h>
#include <iostream>

#include <core/contract.h>
#include <core/scope_guard.h>
#include <stdexcept>

#include <gl/opengl.h>
#include <GL/glx.h>
//#include <cairo.h>
//#include <cairo-gl.h>
//#include <cairo-xlib.h>

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

static bool isExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
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

}

namespace xlib
{

////////////////////////////////////////

window::window( Display *dpy )
	: _display( dpy )
{
	precondition( _display, "null display" );

	// Check GLX version.  Version 1.3 is needed for FBConfig
	int glx_major, glx_minor;
	if ( !glXQueryVersion( _display, &glx_major, &glx_minor ) )
		throw std::runtime_error( "glx query version failed" );
	if ( ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
		throw std::runtime_error( "glx too old" );

	// Get the framebuffer configs
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig( _display, DefaultScreen( _display ), visual_attribs, &fbcount );
	if ( fbc == nullptr )
		throw std::runtime_error( "failed to get GL framebuffer configs" );
	on_scope_exit += [&]() { XFree( fbc ); };

	// Find the best framebuffer
	int best_fbc = -1, best_num_samp = -1;
	for ( int i = 0; i < fbcount; ++i )
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( _display, fbc[i] );
		on_scope_exit += [&]() { XFree( vi ); };

		if ( vi != nullptr )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib( _display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( _display, fbc[i], GLX_SAMPLES, &samples );
			if ( best_fbc < 0 || ( samp_buf && samples > best_num_samp ) )
				best_fbc = i, best_num_samp = samples;
		}
	}

	GLXFBConfig bestFbc = fbc[ best_fbc ];

	XVisualInfo *vi = glXGetVisualFromFBConfig( _display, bestFbc );
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

	Window root = DefaultRootWindow( _display );
	swa.colormap = XCreateColormap( _display, root, vi->visual, AllocNone );

	_win = XCreateWindow( _display, root, 0, 0, 320, 240, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );

	// Get the default screen's GLX extension list
	const char *glxExts = glXQueryExtensionsString( _display, DefaultScreen( _display ) );

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	auto glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	// Check for the GLX_ARB_create_context extension string and the function.
	if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB )
	{
		// If either is not present, use GLX 1.3 context creation method.
		_glc = glXCreateNewContext( _display, bestFbc, GLX_RGBA_TYPE, 0, True );
	}
	else
	{
		// If it does, try to get a GL 4.0 context!
		int context_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};

		_glc = glXCreateContextAttribsARB( _display, bestFbc, 0, True, context_attribs );
	}

	// Sync to ensure any errors generated are processed.
	XSync( _display, False );
}

////////////////////////////////////////

window::~window( void )
{
}

////////////////////////////////////////

void window::raise( void )
{
	XRaiseWindow( _display, _win );
}

////////////////////////////////////////

void window::lower( void )
{
	XLowerWindow( _display, _win );
}

////////////////////////////////////////

void window::show( void )
{
	XMapWindow( _display, _win );
}

////////////////////////////////////////

void window::hide( void )
{
	XUnmapWindow( _display, _win );
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
	XResizeWindow( _display, _win, (unsigned int)( std::max( 0.0, w ) + 0.5 ), (unsigned int)( std::max( 0.0, h ) + 0.5 ) );
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	XStoreName( _display, _win, t.c_str() );
}

////////////////////////////////////////

void window::invalidate( const draw::rect &r )
{
	XClearArea( _display, _win, std::floor( r.x() ), std::floor( r.y() ), std::ceil( r.width() ), std::ceil( r.height() ), true );
}

////////////////////////////////////////

gl::context window::context( void )
{
	glXMakeCurrent( _display, _win, _glc );
	return gl::context();
}

////////////////////////////////////////

std::shared_ptr<draw::canvas> window::canvas( void )
{
	/*
	if ( !_canvas )
	{
		_canvas = std::make_shared<cairo::canvas>();

		Window root;
		int x, y;
		unsigned int w, h;
		unsigned int border, depth;
		XGetGeometry( _display, _win, &root, &x, &y, &w, &h, &border, &depth );
		update_canvas( w, h );
	}
	*/
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
		glXMakeCurrent( _display, _win, _glc );
		glViewport( 0, 0, w, h );
		resized( w, h );
	}
}

////////////////////////////////////////

void window::expose_event( void )
{
	glXMakeCurrent( _display, _win, _glc );
	exposed();
	glXSwapBuffers( _display, _win );
	XFlush( _display );
}

////////////////////////////////////////

}

