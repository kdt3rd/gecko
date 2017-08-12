//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <cstdlib>
#include <dlfcn.h>
#include <mutex>
#include <X11/Xatom.h>

#include <base/contract.h>

namespace
{
// don't rely on xrandr header to minimize dependencies...
typedef struct 
{
	int w, h;
	int mw, mh;
} xrrsz;
typedef xrrsz *(*XRRSizesFunc)(Display *, int, int *);
typedef short *(*XRRRatesFunc)(Display *, int, int, int *);

std::once_flag xrandr_dso_flag;
void *xrandr_dso = nullptr;
XRRSizesFunc xrandr_getsizes = nullptr;
XRRRatesFunc xrandr_getrates = nullptr;

void shutdown_xrandr( void )
{
	if ( xrandr_dso )
	{
		xrandr_getsizes = nullptr;
		xrandr_getrates = nullptr;

		dlclose( xrandr_dso );
		xrandr_dso = nullptr;
	}
}

void init_xrandr( Display *d )
{
	xrandr_dso = dlopen( "libXrandr.so", RTLD_GLOBAL | RTLD_LAZY );
	if ( xrandr_dso )
	{
		int maj, min;
		int evB, erB;
		typedef Bool (*_XRRQueryExtFunc)( Display *, int *, int * );
		typedef Status (*_XRRQueryVerFunc)( Display *, int *, int * );
		_XRRQueryExtFunc qFuncExt = (_XRRQueryExtFunc)dlsym( xrandr_dso, "XRRQueryExtension" );
		_XRRQueryVerFunc qFuncVer = (_XRRQueryVerFunc)dlsym( xrandr_dso, "XRRQueryVersion" );
		if ( qFuncExt && qFuncVer &&
			 (*qFuncExt)( d, &evB, &erB ) &&
			 (*qFuncVer)( d, &maj, &min ) == Success )
		{
			std::cout << "Found active XRandR X extension v" << maj << '.' << min << std::endl;
			// until we support changing the resolution, don't
			// bother querying all the other bits
//			if ( maj > 1 || ( maj == 1 && min >= 2 ) )
//			{
//				_xrandr_ccc = dlsym( "XRRConfigCurrentConfiguration" );
//				_xrandr_ccr = dlsym( "XRRConfigCurrentRate" );
//				_xrandr_free_sci = dlsym( "XRRFreeScreenConfigInfo" );
//				_xrandr_gsi = dlsym( "XRRGetScreenInfo" );
//				_xrandr_free_crtc = dlsym(  );
//				_xrandr_free_sr = dlsym( "XRRFreeScreenResources" );
//			}
		}
		// these are always valid (whether randr is active)...
		xrandr_getsizes = (XRRSizesFunc) dlsym( xrandr_dso, "XRRSizes" );
		xrandr_getrates = (XRRRatesFunc) dlsym( xrandr_dso, "XRRRates" );

		atexit( &shutdown_xrandr );
	}
}

}

namespace platform { namespace xlib
{

////////////////////////////////////////

screen::screen( const std::shared_ptr<Display> &disp, int scr )
	: _display( disp ), _screen( scr )
{
	// like similar dynamic-loaded DSO for X, like OpenGL, need to
	// bind this into the global space, resolve symbols
	// but then NOT dlclose it until after the display is fully
	// closed...
	std::call_once( xrandr_dso_flag, [&]() { init_xrandr( disp.get() ); } );
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

bool screen::is_default( void ) const
{
	return DefaultScreen( _display.get() ) == _screen;
}

////////////////////////////////////////

double
screen::refresh_rate( void ) const
{
	if ( xrandr_getrates )
	{
		int nrates = 0;
		short *rates = xrandr_getrates( _display.get(), _screen, 0, &nrates );
		if ( rates )
			return static_cast<double>(rates[0]);
	}

	return 0.F;
}

////////////////////////////////////////

base::rect screen::bounds( bool avail ) const
{
	if ( avail )
	{
        Atom hints = XInternAtom( _display.get(), "_NET_WORKAREA", True );
		Window root = RootWindow( _display.get(), _screen );
		unsigned long nitems, bytesLeft;
		Atom actualType;
		int actualFormat;
		long *workarea = nullptr;
		bool success = false;
		base::rect ret;
		if ( XGetWindowProperty( _display.get(), root, hints, 0, 4, False,
								 XA_CARDINAL, &actualType, &actualFormat, &nitems, &bytesLeft,
								 (unsigned char**)&workarea ) == Success )
		{
			if ( actualType == XA_CARDINAL && actualFormat == 32 && nitems == 4 )
			{
				ret = base::rect( static_cast<double>( workarea[0] ),
								  static_cast<double>( workarea[1] ),
								  static_cast<double>( workarea[2] ),
								  static_cast<double>( workarea[3] ) );
				success = true;
            }
			if ( workarea )
                XFree( workarea );
        }

		if ( success )
			return ret;
	}

	if ( xrandr_getsizes )
	{
		int nsizes = 0;
		xrrsz *sizes = xrandr_getsizes( _display.get(), _screen, &nsizes );
		if ( sizes )
			return base::rect( 0.0, 0.0,
							   static_cast<double>(sizes[0].w),
							   static_cast<double>(sizes[0].h) );
	}

	return base::rect( 0.0, 0.0,
					   static_cast<double>( DisplayWidth( _display.get(), _screen ) ),
					   static_cast<double>( DisplayHeight( _display.get(), _screen ) ) );
}

////////////////////////////////////////

base::size screen::dpi( void ) const
{
	if ( xrandr_getsizes )
	{
		int nsizes = 0;
		xrrsz *sizes = xrandr_getsizes( _display.get(), _screen, &nsizes );
		if ( sizes )
			return { static_cast<double>(sizes[0].w) * 25.4 / static_cast<double>(sizes[0].mw),
					 static_cast<double>(sizes[0].h) * 25.4 / static_cast<double>(sizes[0].mh)
					};
	}

	double tmpW = ( ( static_cast<double>( DisplayWidth( _display.get(), _screen ) ) *
					  25.4 ) /
					static_cast<double>( DisplayWidthMM( _display.get(), _screen ) ) );
	double tmpH = ( ( static_cast<double>( DisplayHeight( _display.get(), _screen ) ) *
					  25.4 ) /
					static_cast<double>( DisplayHeightMM( _display.get(), _screen ) ) );
	return { tmpW, tmpH };
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform

