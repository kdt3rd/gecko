
#include "system.h"
#include "screen.h"
#include "window.h"

#include <stdexcept>
#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

system::system( void )
	: platform::system( "sdl2", "SDL 2.0" )
{
	if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
		throw std::runtime_error( SDL_GetError() );

	for ( int i = 0; i < SDL_GetNumVideoDisplays(); ++i )
		_screens.push_back( std::make_shared<screen>( i ) );
}

////////////////////////////////////////

system::~system( void )
{
	SDL_Quit();
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	return std::make_shared<window>();
}

////////////////////////////////////////

}
