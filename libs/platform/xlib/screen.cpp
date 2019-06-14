// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "screen.h"

#include <X11/Xatom.h>
#include <base/contract.h>
#include <cstdlib>
#include <dlfcn.h>
#include <mutex>

namespace
{
// don't rely on xrandr header to minimize dependencies...
typedef struct
{
    int w, h;
    int mw, mh;
} xrrsz;
typedef xrrsz *( *XRRSizesFunc )( Display *, int, int * );
typedef short *( *XRRRatesFunc )( Display *, int, int, int * );
typedef void ( *XRRSelectInputFunc )( Display *, Window w, int );

std::once_flag     xrandr_dso_flag;
void *             xrandr_dso      = nullptr;
XRRSizesFunc       xrandr_getsizes = nullptr;
XRRRatesFunc       xrandr_getrates = nullptr;
XRRSelectInputFunc xrandr_selinput = nullptr;
int                _ev_base        = -1;

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
        typedef Bool ( *_XRRQueryExtFunc )( Display *, int *, int * );
        typedef Status ( *_XRRQueryVerFunc )( Display *, int *, int * );
        _XRRQueryExtFunc qFuncExt =
            (_XRRQueryExtFunc)dlsym( xrandr_dso, "XRRQueryExtension" );
        _XRRQueryVerFunc qFuncVer =
            (_XRRQueryVerFunc)dlsym( xrandr_dso, "XRRQueryVersion" );
        if ( qFuncExt && qFuncVer && ( *qFuncExt )( d, &evB, &erB ) &&
             ( *qFuncVer )( d, &maj, &min ) == Success )
        {
            _ev_base = evB;
            std::cout << "Found active XRandR X extension v" << maj << '.'
                      << min << std::endl;
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
        xrandr_getsizes = (XRRSizesFunc)dlsym( xrandr_dso, "XRRSizes" );
        xrandr_getrates = (XRRRatesFunc)dlsym( xrandr_dso, "XRRRates" );
        xrandr_selinput =
            (XRRSelectInputFunc)dlsym( xrandr_dso, "XRRSelectInput" );

        atexit( &shutdown_xrandr );
    }
}

} // namespace

namespace platform
{
namespace xlib
{
////////////////////////////////////////

screen::screen( const std::shared_ptr<Display> &disp, int scr )
    : _display( disp )
    , _screen( scr )
    , _standard( color::make_standard( color::standard::SRGB ) )
{
    // like similar dynamic-loaded DSO for X, like OpenGL, need to
    // bind this into the global space, resolve symbols
    // but then NOT dlclose it until after the display is fully
    // closed...
    std::call_once( xrandr_dso_flag, [&]() { init_xrandr( disp.get() ); } );

    if ( xrandr_selinput )
        xrandr_selinput(
            disp.get(),
            RootWindow( disp.get(), scr ),
            ( 1L << 0 ) /*RRScreenChangeNotifyMask*/ );
    update_resolution();
}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

bool screen::is_default( void ) const
{
    return DefaultScreen( _display.get() ) == _screen;
}

////////////////////////////////////////

bool screen::is_managed( void ) const { return true; }

////////////////////////////////////////

bool screen::is_remote( void ) const { return false; }

////////////////////////////////////////

rect screen::bounds( bool avail ) const
{
    if ( avail )
    {
        Atom   hints = XInternAtom( _display.get(), "_NET_WORKAREA", True );
        Window root  = RootWindow( _display.get(), _screen );
        unsigned long nitems, bytesLeft;
        Atom          actualType;
        int           actualFormat;
        long *        workarea = nullptr;
        bool          success  = false;
        rect          ret;
        if ( XGetWindowProperty(
                 _display.get(),
                 root,
                 hints,
                 0,
                 4,
                 False,
                 XA_CARDINAL,
                 &actualType,
                 &actualFormat,
                 &nitems,
                 &bytesLeft,
                 (unsigned char **)&workarea ) == Success )
        {
            if ( actualType == XA_CARDINAL && actualFormat == 32 &&
                 nitems == 4 )
            {
                ret = rect(
                    static_cast<coord_type>( workarea[0] ),
                    static_cast<coord_type>( workarea[1] ),
                    static_cast<coord_type>( workarea[2] ),
                    static_cast<coord_type>( workarea[3] ) );
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
        int    nsizes = 0;
        xrrsz *sizes  = xrandr_getsizes( _display.get(), _screen, &nsizes );
        if ( sizes )
            return rect(
                coord_type( 0 ),
                coord_type( 0 ),
                static_cast<coord_type>( sizes[0].w ),
                static_cast<coord_type>( sizes[0].h ) );
    }

    return rect(
        coord_type( 0 ),
        coord_type( 0 ),
        static_cast<coord_type>( DisplayWidth( _display.get(), _screen ) ),
        static_cast<coord_type>( DisplayHeight( _display.get(), _screen ) ) );
}

////////////////////////////////////////

dots_per_unit screen::dpi( void ) const
{
    dots_per_unit dmm = dpmm();
    return dots_per_unit(
        dmm.w() * dots_per_unit::coord_type( 25.4 ),
        dmm.h() * dots_per_unit::coord_type( 25.4 ) );
}

////////////////////////////////////////

int screen::resolution_event_id() { return _ev_base; }

////////////////////////////////////////

void screen::update_resolution( void )
{
    using val_type = dots_per_unit::coord_type;

    if ( xrandr_getrates )
    {
        int    nrates = 0;
        short *rates  = xrandr_getrates( _display.get(), _screen, 0, &nrates );
        if ( rates )
            _d_rr = static_cast<double>( rates[0] );
        else
            _d_rr = 0.0;
    }
    else
        _d_rr = 0.0;

    if ( xrandr_getsizes )
    {
        int    nsizes = 0;
        xrrsz *sizes  = xrandr_getsizes( _display.get(), _screen, &nsizes );
        if ( sizes )
        {
            _d_p_mm.set(
                static_cast<val_type>( sizes[0].w ) /
                    static_cast<val_type>( sizes[0].mw ),
                static_cast<val_type>( sizes[0].h ) /
                    static_cast<val_type>( sizes[0].mh ) );
            return;
        }
    }

    _d_p_mm.set(
        ( static_cast<val_type>( DisplayWidth( _display.get(), _screen ) ) /
          static_cast<val_type>( DisplayWidthMM( _display.get(), _screen ) ) ),
        ( static_cast<val_type>( DisplayHeight( _display.get(), _screen ) ) /
          static_cast<val_type>(
              DisplayHeightMM( _display.get(), _screen ) ) ) );
}

////////////////////////////////////////

double screen::refresh_rate( void ) const
{
    if ( xrandr_getrates )
    {
        int    nrates = 0;
        short *rates  = xrandr_getrates( _display.get(), _screen, 0, &nrates );
        if ( rates )
            return static_cast<double>( rates[0] );
    }

    return 0.F;
}

////////////////////////////////////////

const color::standard_definition &screen::display_standard( void ) const
{
    return _standard;
}

////////////////////////////////////////

void screen::override_display_standard( const color::standard_definition &s )
{
    _standard = s;
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform
