//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "timer.h"

namespace platform
{

////////////////////////////////////////

timer::timer( void )
{
}

////////////////////////////////////////

timer::~timer( void )
{
}

////////////////////////////////////////

void
timer::activate( bool on )
{
	_active = on;
}

////////////////////////////////////////

void
timer::reset_one_off( double dur )
{
	std::chrono::duration<double, std::milli> fd( dur * 1000.0 );
	reset_one_off( clock::now() + std::chrono::duration_cast<duration>( fd ) );
}

////////////////////////////////////////

void
timer::reset_one_off( duration dur )
{
	reset_one_off( clock::now() + dur );
}

////////////////////////////////////////

void
timer::reset_one_off( time_point w )
{
	_repeat_dur = duration(-1);
	_expire_time = w;
}

////////////////////////////////////////

void
timer::reset_repeat( double dur )
{
	std::chrono::duration<double, std::milli> fd( dur * 1000.0 );
	reset_repeat( std::chrono::duration_cast<duration>( fd ) );
}

////////////////////////////////////////

void
timer::reset_repeat( duration dur )
{
	_repeat_dur = dur;
	_expire_time = clock::now() + dur;
}

////////////////////////////////////////

void
timer::start( void )
{
}

////////////////////////////////////////

void
timer::cancel( void )
{
	_active = false;
}

////////////////////////////////////////

void
timer::shutdown( void )
{
	_active = false;
}

////////////////////////////////////////

waitable::wait
timer::poll_object( void )
{
	return INVALID_WAIT;
}

////////////////////////////////////////

bool
timer::poll_timeout( std::chrono::high_resolution_clock::duration &when, const std::chrono::high_resolution_clock::time_point &curtime )
{
	if ( _active )
	{
		when = _expire_time - curtime;
		return true;
	}
	return false;
}

////////////////////////////////////////

void
timer::emit( const std::chrono::high_resolution_clock::time_point &curtime )
{
	if ( _active && _expire_time <= curtime )
	{
		// actuall fire the timer
		if ( elapsed )
			elapsed();

		// one-off, deactivate
		if ( _repeat_dur < duration::zero() )
			activate( false );
		else // otherwise set up repeat
			_expire_time = curtime + _repeat_dur;
	}
}

////////////////////////////////////////

} // namespace platform
