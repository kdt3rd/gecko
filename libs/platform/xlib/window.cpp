//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "window.h"
#include <base/pointer.h>

#include <string.h>
#include <iostream>

#include <base/contract.h>
#include <base/scope_guard.h>
#include <stdexcept>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "system.h"
#include "context.h"
#include "cursor.h"

namespace {

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

namespace platform { namespace xlib
{

////////////////////////////////////////

window::window( system &s, const std::shared_ptr<Display> &dpy, const std::shared_ptr<::platform::screen> &scr, const rect &p )
	: ::platform::window( scr, p ), _display( dpy )
{
	precondition( _display, "null display" );

	Display *disp = _display.get();

	_ctxt = std::make_shared<context>( dpy );

	XVisualInfo *vi = _ctxt->choose_best_config();
	if ( vi == nullptr )
		throw std::runtime_error( "no visual found" );
	on_scope_exit { XFree( vi ); };

	XSetWindowAttributes swa;
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask =
		KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask | PointerMotionMask | KeymapStateMask | ExposureMask |
		VisibilityChangeMask | StructureNotifyMask |
		FocusChangeMask |
		PropertyChangeMask | ColormapChangeMask | OwnerGrabButtonMask;
	// | PointerMotionHintMask |
	//Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask |
	//	Button5MotionMask | ButtonMotionMask
		//ResizeRedirectMask |
		//SubstructureNotifyMask | SubstructureRedirectMask |

	Window root = DefaultRootWindow( disp );
	swa.colormap = XCreateColormap( disp, root, vi->visual, AllocNone );

	_win = XCreateWindow( disp, root, x(), y(), width(), height(), 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );

	_ctxt->create( _win );

//	std::cout << "OpenGL:\n\tvendor " << glGetString( GL_VENDOR )
//			  << "\n\trenderer " << glGetString( GL_RENDERER )
//			  << "\n\tversion " << glGetString( GL_VERSION )
//			  << "\n\tshader language " << glGetString( GL_SHADING_LANGUAGE_VERSION )
//			  << "\n\n" << std::endl;

//	int extCount;
//	glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
//	std::cout << extCount << " extensions:\n";
//	for (int i = 0; i < extCount; ++i)
//		std::cout << "Extension " << i << ": " << glGetStringi( GL_EXTENSIONS, i ) << '\n';
//	std::cout << std::endl;

	// Sync to ensure any errors generated are processed.
	XSync( disp, False );
}

////////////////////////////////////////

window::~window( void )
{
	// shut this down first before we kill the window...
	_ctxt.reset();
	if ( _win != 0 )
		XDestroyWindow( _display.get(), _win );
}

////////////////////////////////////////

::platform::context &window::hw_context( void )
{
	return *(_ctxt);
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

	Atom winType = XInternAtom( _display.get(), "_NET_WM_WINDOW_TYPE", False );
	Atom defType = XInternAtom( _display.get(), "_NET_WM_WINDOW_TYPE_POPUP_MENU", False );
	XChangeProperty( _display.get(), _win, winType, XA_ATOM, 32, PropModeReplace,
					 reinterpret_cast<unsigned char *>( &defType ), 1 );
	_popup = true;
}

////////////////////////////////////////

void window::show( void )
{
	XMapWindow( _display.get(), _win );
	if ( _popup )
		XRaiseWindow( _display.get(), _win );
}

////////////////////////////////////////

void window::hide( void )
{
	XUnmapWindow( _display.get(), _win );
}

////////////////////////////////////////

bool window::is_visible( void )
{
	XWindowAttributes attr;	  
	XGetWindowAttributes( _display.get(), _win, &attr );
	return (attr.map_state == IsViewable);
}

////////////////////////////////////////

void
window::fullscreen( bool fs )
{
	// look at ghost - need to handle netwm method and motif method...
}

////////////////////////////////////////

void window::apply_minimum_size( coord_type w, coord_type h )
{
	_min_w = w;
	_min_h = h;
	rect r = query_geometry();
	bool changed = false;
	if ( r.width() < _min_w )
	{
		r.set_width( _min_w );
		changed = true;
	}
	if ( r.height() < _min_h )
	{
		r.set_height( _min_h );
		changed = true;
	}

	if ( changed )
		update_geometry( r );
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	XStoreName( _display.get(), _win, t.c_str() );
}

////////////////////////////////////////

Window window::id( void ) const
{
	return _win;
}

////////////////////////////////////////

void window::submit_delayed_expose( const rect &r )
{
	XExposeEvent exp = { Expose, 0, 1, _display.get(), _win, r.x(), r.y(), r.width(), r.height(), 0 };
	XSendEvent( _display.get(), _win, False, ExposureMask, reinterpret_cast<XEvent *>( &exp ) );
	XFlush( _display.get() );
}

////////////////////////////////////////

rect window::query_geometry( void )
{
	int x = 0, y = 0;
	unsigned int w = 0, h = 0;
	unsigned int bw = 0, d = 0;
	Window root = None;
	rect ret;
	if ( XGetGeometry( _display.get(), _win, &root, &x, &y, &w, &h, &bw, &d ) )
	{
		ret.set( static_cast<coord_type>( x ), static_cast<coord_type>( y ),
				 static_cast<coord_type>( w ), static_cast<coord_type>( h ) );
	}
	return ret;
}

////////////////////////////////////////

bool window::update_geometry( rect &r )
{
	if ( r.width() < _min_w )
		r.set_width( _min_w );
	if ( r.height() < _min_h )
		r.set_height( _min_h );

	int x = r.x(), y = r.y();
	unsigned int w = static_cast<unsigned int>( r.width() );
	unsigned int h = static_cast<unsigned int>( r.height() );
	XMoveResizeWindow( _display.get(), _win, x, y, w, h );

	return true;
}

////////////////////////////////////////

void window::make_current( const std::shared_ptr<::platform::cursor> &c )
{
	if ( c )
	{
		auto xc = static_cast<::platform::xlib::cursor *>( c.get() );
		XDefineCursor( _display.get(), _win, xc->handle() );
	}
	else
	{
		// no cursor - just inherit parent window cursor
		XUndefineCursor( _display.get(), _win );
	}

	XFlush( _display.get() );
}

////////////////////////////////////////

} }

