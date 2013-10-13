
#include "screen.h"

#include <stdexcept>

#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

screen::screen( int idx )
	: _index( idx )
{
}

////////////////////////////////////////

screen::~screen( void )
{
}

////////////////////////////////////////

size screen::bounds( void )
{
	SDL_Rect rect;

	if ( SDL_GetDisplayBounds( _index, &rect ) == 0 )
		return size( rect.w, rect.h );

	throw std::runtime_error( SDL_GetError() );
}

////////////////////////////////////////

}

