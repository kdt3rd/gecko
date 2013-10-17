
#pragma once

#include <platform/timer.h>
#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

class timer : public platform::timer
{
public:
	timer( void );
	virtual ~timer( void );

	virtual void schedule( double seconds );
	virtual void cancel( void );

private:
	static Uint32 elapse_timer( Uint32, void *t );

	double _timeout = 0.0;
	SDL_TimerID _timer = 0;
};

////////////////////////////////////////

}

