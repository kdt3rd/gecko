
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

size screen::bounds( void )
{
//	precondition( _screen, "null screen" );
	return size( 0, 0 );
}

////////////////////////////////////////

}

