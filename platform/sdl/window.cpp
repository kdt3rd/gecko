
#include "window.h"
#include "painter.h"
#include <stdexcept>

namespace sdl
{

////////////////////////////////////////

window::window( void )
{
	if ( SDL_CreateWindowAndRenderer( 0, 0, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE, &_window, &_render ) != 0 )
		throw std::runtime_error( SDL_GetError() );
}

////////////////////////////////////////

window::~window( void )
{
	SDL_DestroyRenderer( _render );
	SDL_DestroyWindow( _window );
	_render = nullptr;
	_window = nullptr;
}

////////////////////////////////////////

void window::raise( void )
{
	SDL_RaiseWindow( _window );
}

////////////////////////////////////////

/*
void window::lower( void )
{
}
*/

////////////////////////////////////////

void window::show( void )
{
	SDL_ShowWindow( _window );
}

////////////////////////////////////////

void window::hide( void )
{
	SDL_HideWindow( _window );
}

////////////////////////////////////////

bool window::is_visible( void )
{
	return SDL_GetWindowFlags( _window ) & SDL_WINDOW_SHOWN;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::resize( double w, double h )
{
	SDL_SetWindowSize( _window, w, h );
}

////////////////////////////////////////

void window::set_minimum_size( double w, double h )
{
	SDL_SetWindowMinimumSize( _window, w, h );
}

////////////////////////////////////////

void window::set_title( const std::string &t )
{
	SDL_SetWindowTitle( _window, t.c_str() );
}

////////////////////////////////////////

std::shared_ptr<platform::painter> window::paint( void )
{
	return std::make_shared<painter>( _render );
}

////////////////////////////////////////

uint32_t window::id( void ) const
{
	return SDL_GetWindowID( _window );
}

////////////////////////////////////////

}

