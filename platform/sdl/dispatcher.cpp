
#include "dispatcher.h"

namespace sdl
{

////////////////////////////////////////

dispatcher::dispatcher( void )
{
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;

	bool done = false;
	while ( !done )
	{
		SDL_Event event;
		if ( SDL_WaitEvent( &event ) == 0 )
			throw std::runtime_error( SDL_GetError() );

		switch ( event.type )
		{
			case SDL_QUIT:
				done = true;
				break;
		}
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;

	SDL_Event event;
	event.type = SDL_QUIT;
	if ( SDL_PushEvent( &event) == 0 )
		throw std::runtime_error( SDL_GetError() );
}

////////////////////////////////////////

void dispatcher::add_window( const std::shared_ptr<window> &w )
{
	_windows[w->id()] = w;
}

////////////////////////////////////////

}

