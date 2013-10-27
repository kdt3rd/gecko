
#include "screen.h"

#include <core/contract.h>
#include <string>
#include <stdexcept>
#include <xcb/xcb.h>

namespace cocoa
{

////////////////////////////////////////

screen::screen( void )
{
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

size screen::bounds( void )
{
	return size( 0, 0 );
}

////////////////////////////////////////

}

