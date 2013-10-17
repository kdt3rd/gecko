
#include <core/contract.h>
#include "timer.h"

namespace sdl
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
	if ( _timeout > 0.0 )
		_timer = SDL_AddTimer( Uint32( _timeout * 1000 + 0.5 ), &elapse_timer, this );
}

////////////////////////////////////////

void timer::cancel( void )
{
	if ( _timer > 0 )
		SDL_RemoveTimer( _timer );
	_timer = 0;
	_timeout = 0.0;
}

////////////////////////////////////////

Uint32 timer::elapse_timer( Uint32, void *vt )
{
	precondition( vt, "null timer elapsed" );
	timer *t = static_cast<timer *>( vt );
	t->_timeout = 0.0;
	t->elapsed();
	return Uint32( t->_timeout * 1000 + 0.5 );
}

////////////////////////////////////////

}

