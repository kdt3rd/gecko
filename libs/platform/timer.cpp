//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

intptr_t
timer::poll_object( void )
{
	return intptr_t(-1);
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
