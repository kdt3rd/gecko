
#include "screen.h"

#include <base/contract.h>
#include <string>
#include <stdexcept>

namespace platform { namespace xlib
{

////////////////////////////////////////

screen::screen( const std::shared_ptr<Display> &disp, int scr )
		: _display( disp ), _screen( scr )
{
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

base::size screen::bounds( void ) const
{
	return { static_cast<double>( DisplayWidth( _display.get(), _screen ) ),
			static_cast<double>( DisplayHeight( _display.get(), _screen ) ) };
}

////////////////////////////////////////

base::size screen::dpi( void ) const
{
	double tmpW = ( ( static_cast<double>( DisplayWidth( _display.get(), _screen ) ) *
					  25.4 ) /
					static_cast<double>( DisplayWidthMM( _display.get(), _screen ) ) );
	double tmpH = ( ( static_cast<double>( DisplayHeight( _display.get(), _screen ) ) *
					  25.4 ) /
					static_cast<double>( DisplayHeightMM( _display.get(), _screen ) ) );
	return { tmpW, tmpH };
}

////////////////////////////////////////

} }

