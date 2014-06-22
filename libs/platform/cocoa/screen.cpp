
#include "screen.h"

#include <base/contract.h>
#include <string>
#include <stdexcept>

namespace platform { namespace cocoa
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

base::size screen::bounds( void ) const
{
	return { 0, 0 };
}

////////////////////////////////////////

} }


