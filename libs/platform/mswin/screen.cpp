//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "screen.h"

#include <string>
#include <stdexcept>
#include <base/contract.h>
#include <base/scope_guard.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

screen::screen( HMONITOR mon )
	: _monitor( mon )
{
	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);

	GetMonitorInfo( mon, &mi );
	_disp_devname = std::string( reinterpret_cast<const char *>( mi.szDevice ) );
	_is_primary = ( mi.dwFlags & MONITORINFOF_PRIMARY ) != 0;
	std::cout << "found display '" << _disp_devname << "'" << std::endl;
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

bool
screen::is_default( void ) const
{
	return _is_primary;
}

////////////////////////////////////////

bool screen::is_managed( void ) const
{
	return true;
}

////////////////////////////////////////

bool screen::is_remote( void ) const
{
	return false;
}

////////////////////////////////////////

rect screen::bounds( bool avail ) const
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo( _monitor, &mi );

	RECT sz;
	if ( avail )
		sz = mi.rcWork;
	else
		sz = mi.rcMonitor;

	return rect( static_cast<coord_type>( sz.left ),
				 static_cast<coord_type>( sz.top ),
				 static_cast<coord_type>( sz.right - sz.left + 1 ),
				 static_cast<coord_type>( sz.bottom - sz.top + 1 ) );
}

////////////////////////////////////////

double
screen::refresh_rate( void ) const
{
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	if ( EnumDisplaySettings( (LPCTSTR)_disp_devname.c_str(),
							  ENUM_CURRENT_SETTINGS, &dm ) )
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

base::dsize screen::dpi( void ) const
{
	HDC dc = CreateDC( TEXT("DISPLAY"), (LPCTSTR)_disp_devname.c_str(), NULL, NULL );
	if ( dc == NULL )
		throw_lasterror( "unable to create device for DPI query" );
	on_scope_exit{ DeleteDC( dc ); };

	// logpixelsx/y returns same value for all screens...
	double bx, by;
	bx = static_cast<double>( GetDeviceCaps( dc, HORZRES ) );
	by = static_cast<double>( GetDeviceCaps( dc, VERTRES ) );

	int szx = GetDeviceCaps( dc, HORZSIZE );
	int szy = GetDeviceCaps( dc, VERTSIZE );
	bx *= 25.4 / static_cast<double>( szx );
	by *= 25.4 / static_cast<double>( szy );

	return { bx, by };
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

