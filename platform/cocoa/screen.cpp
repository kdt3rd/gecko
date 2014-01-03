
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

draw::size screen::bounds( void )
{
	return { 0, 0 };
}

////////////////////////////////////////

}

