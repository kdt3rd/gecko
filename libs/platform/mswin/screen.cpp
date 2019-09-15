// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "screen.h"

#include <base/contract.h>
#include <base/scope_guard.h>
#include <stdexcept>
#include <string>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

screen::screen( HMONITOR mon ) : _monitor( mon )
{
    MONITORINFOEX mi;
    mi.cbSize = sizeof( mi );

    GetMonitorInfo( mon, &mi );
    _disp_devname =
        std::string( reinterpret_cast<const char *>( mi.szDevice ) );
    _is_primary = ( mi.dwFlags & MONITORINFOF_PRIMARY ) != 0;
    std::cout << "found display '" << _disp_devname << "'" << std::endl;
}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

bool screen::is_default( void ) const { return _is_primary; }

////////////////////////////////////////

bool screen::is_managed( void ) const { return true; }

////////////////////////////////////////

bool screen::is_remote( void ) const { return false; }

////////////////////////////////////////

rect screen::bounds( bool avail ) const
{
    MONITORINFO mi;
    mi.cbSize = sizeof( mi );
    GetMonitorInfo( _monitor, &mi );

    RECT sz;
    if ( avail )
        sz = mi.rcWork;
    else
        sz = mi.rcMonitor;

    return rect(
        static_cast<coord_type>( sz.left ),
        static_cast<coord_type>( sz.top ),
        static_cast<coord_type>( sz.right - sz.left + 1 ),
        static_cast<coord_type>( sz.bottom - sz.top + 1 ) );
}

////////////////////////////////////////

double screen::refresh_rate( void ) const
{
    DEVMODE dm;
    dm.dmSize        = sizeof( dm );
    dm.dmDriverExtra = 0;
    if ( EnumDisplaySettings(
             (LPCTSTR)_disp_devname.c_str(), ENUM_CURRENT_SETTINGS, &dm ) )
    {
        // comment says this may be 0 or 1 indicating the
        // 'default' hardware frequency for devices not managed
        // by the system normally...
        if ( ( dm.dmFields & DM_DISPLAYFREQUENCY ) != 0 &&
             dm.dmDisplayFrequency > 1 )
            return static_cast<double>( dm.dmDisplayFrequency );
    }
    return 0.0;
}

////////////////////////////////////////

dots_per_unit screen::dpi( void ) const
{
    dots_per_unit mm = dpmm();

    return dots_per_unit(
        mm.w() * dots_per_unit::coord_type( 25.4 ),
        mm.h() * dots_per_unit::coord_type( 25.4 ) );
}

////////////////////////////////////////

dots_per_unit screen::dpmm( void ) const
{
    HDC dc = CreateDC(
        TEXT( "DISPLAY" ), (LPCTSTR)_disp_devname.c_str(), NULL, NULL );
    if ( dc == NULL )
        throw_lasterror( "unable to create device for DPI query" );
    on_scope_exit { DeleteDC( dc ); };

    // logpixelsx/y returns same value for all screens...
    return dots_per_unit(
        ( dots_per_unit::coord_type( GetDeviceCaps( dc, HORZRES ) ) /
          dots_per_unit::coord_type( GetDeviceCaps( dc, HORZSIZE ) ) ),
        dots_per_unit::coord_type( GetDeviceCaps( dc, VERTRES ) ) /
            dots_per_unit::coord_type( GetDeviceCaps( dc, VERTSIZE ) ) );
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

} // namespace mswin
} // namespace platform
