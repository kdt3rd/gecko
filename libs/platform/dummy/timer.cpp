
#include <string>
#include <base/contract.h>
#include "timer.h"

namespace platform { namespace dummy
{

////////////////////////////////////////

timer::timer( void )
{
}

////////////////////////////////////////

timer::~timer( void )
{
	cancel();
}

////////////////////////////////////////

void timer::schedule( double seconds )
{
	precondition( _timeout == 0.0, "timer alread schedule" );
	precondition( seconds >= 0.0, "invalid timer schedule" );

	cancel();
	_timeout = seconds;
}

////////////////////////////////////////

void timer::cancel( void )
{
	_timeout = 0.0;
}

////////////////////////////////////////

} }

