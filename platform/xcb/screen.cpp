
#include "screen.h"

#include <core/contract.h>
#include <string>
#include <stdexcept>
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

screen::screen( xcb_screen_t *scr )
	: _screen( scr )
{
	precondition( _screen, "null screen" );
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

size screen::bounds( void )
{
	precondition( _screen, "null screen" );
	return size( _screen->width_in_pixels, _screen->height_in_pixels );
}

////////////////////////////////////////

}

