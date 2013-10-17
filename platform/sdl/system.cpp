
#include "system.h"
#include "screen.h"
#include "window.h"
#include "timer.h"
#include "dispatcher.h"

#include <stdexcept>
#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

system::system( void )
	: platform::system( "sdl2", "SDL 2.0" )
{
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
		throw std::runtime_error( SDL_GetError() );

	for ( int i = 0; i < SDL_GetNumVideoDisplays(); ++i )
		_screens.push_back( std::make_shared<screen>( i ) );

	_dispatcher = std::make_shared<sdl::dispatcher>();
}

////////////////////////////////////////

system::~system( void )
{
	SDL_Quit();
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>();
	_dispatcher->add_window( ret );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<platform::timer> system::new_timer( void )
{
	return std::make_shared<timer>();
}

////////////////////////////////////////

std::shared_ptr<platform::dispatcher> system::dispatch( void )
{
	return _dispatcher;
}


////////////////////////////////////////

}
