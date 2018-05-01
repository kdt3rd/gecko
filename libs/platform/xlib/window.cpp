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

window::window( system &s, const std::shared_ptr<Display> &dpy, const std::shared_ptr<::platform::screen> &scr )
	: ::platform::window( scr ), _display( dpy )
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

	_win = XCreateWindow( disp, root, 0, 0, 320, 240, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );

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

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::move( coord_type x, coord_type y )
{
	XMoveWindow( _display.get(), _win, static_cast<int>( x ), static_cast<int>( y ) );
}

////////////////////////////////////////

void window::resize( coord_type w, coord_type h )
{
	XResizeWindow( _display.get(), _win,
				   static_cast<unsigned int>( w ),
				   static_cast<unsigned int>( h ) );
}

////////////////////////////////////////

void window::set_minimum_size( coord_type /*w*/, coord_type /*h*/ )
{
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	XStoreName( _display.get(), _win, t.c_str() );
}

////////////////////////////////////////

void window::invalidate( const rect &r )
{
	if ( !_invalid )
	{
		// TODO
		_invalid_rgn.include( r );
		_invalid = true;
		XClearArea( _display.get(), _win, r.x(), r.y(), r.width(), r.height(), true );
	}
}

////////////////////////////////////////

Window window::id( void ) const
{
	return _win;
}

////////////////////////////////////////

void window::move_event( coord_type x, coord_type y )
{
	int16_t tx = static_cast<int16_t>( x );
	int16_t ty = static_cast<int16_t>( y );
	if ( _last_x != tx || _last_y != ty )
	{
		_last_x = tx;
		_last_y = ty;
		if ( moved )
			moved( x, y );
	}
}

////////////////////////////////////////

void window::resize_event( coord_type w, coord_type h )
{
	uint16_t tw = static_cast<uint16_t>( w );
	uint16_t th = static_cast<uint16_t>( h );
	if ( _last_w != tw || _last_h != th )
	{
		_last_w = tw;
		_last_h = th;
		auto guard = _ctxt->begin_render();
		_ctxt->set_viewport( 0, 0, tw, th );
		if ( resized )
			resized( w, h );
	}
}

////////////////////////////////////////

void window::expose_event( coord_type x, coord_type y, coord_type w, coord_type h )
{
	auto guard = _ctxt->begin_render();

	if ( w == 0 && h == 0 )
		_invalid_rgn = rect();
	if ( ( w != 0 && h != 0 ) || ! _invalid_rgn.empty() )
	{
		_invalid_rgn.include( rect( x, y, w, h ) );
//		_glc_enable( GL_SCISSOR_TEST );
//		_glc_scissor( static_cast<GLint>( _invalid_rgn.x() ),
//					  static_cast<GLint>( _last_h - _invalid_rgn.y() ),
//					  static_cast<GLsizei>( _invalid_rgn.width() ),
//					  static_cast<GLsizei>( _invalid_rgn.height() ) );
	}
//	else
//		_glc_disable( GL_SCISSOR_TEST );
	if ( exposed )
		exposed();
	_ctxt->swap_buffers();
//	_glc_disable( GL_SCISSOR_TEST );
	_invalid = false;
	_invalid_rgn = rect();
//	glFlush();
//	XFlush( _display.get() );
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

