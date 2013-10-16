
#pragma once

#include <platform/window.h>
#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

class window : public platform::window
{
public:
	window( void );
	virtual ~window( void );

	virtual void raise( void );
//	virtual void lower( void );

	virtual void show( void );
	virtual void hide( void );
	virtual bool is_visible( void );

//	virtual rect geometry( void );
//	virtual void set_position( double x, double y );
	virtual void resize( double w, double h );
	virtual void set_minimum_size( double w, double h );

	virtual void set_title( const std::string &t );
//	virtual void set_icon( const icon &i );

	virtual std::shared_ptr<platform::painter> paint( void );

	uint32_t id( void ) const;

private:
	SDL_Window *_window;
	SDL_Renderer *_render;
};

////////////////////////////////////////

}

