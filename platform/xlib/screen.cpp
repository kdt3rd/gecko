
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

draw::size screen::bounds( void )
{
//	precondition( _screen, "null screen" );
	return { 0, 0 };
}

////////////////////////////////////////

}

