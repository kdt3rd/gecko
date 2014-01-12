
#include "screen.h"

#include <core/contract.h>
#include <string>
#include <stdexcept>

namespace x11
{

////////////////////////////////////////

screen::screen( void )
{
//	precondition( _screen, "null screen" );
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

core::size screen::bounds( void )
{
	return { 0, 0 };
}

////////////////////////////////////////

}

