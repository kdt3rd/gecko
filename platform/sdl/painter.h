
#pragma once

#include <platform/painter.h>
#include <SDL2/SDL.h>

namespace sdl
{

////////////////////////////////////////

class painter : public platform::painter
{
public:
	painter( SDL_Renderer *r );
	virtual ~painter( void );

	virtual void draw_points( const point *ls, size_t ln );
	virtual void draw_polygon( const point *ps, size_t pn );

	virtual void draw_lines( const line *ps, size_t pn );
	virtual void draw_rects( const rectangle *rs, size_t rn );

	virtual void present( void );
private:
	SDL_Renderer *_render;
};

////////////////////////////////////////

}

