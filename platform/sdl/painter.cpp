
#include <vector>
#include "painter.h"

namespace sdl
{

////////////////////////////////////////

painter::painter( SDL_Renderer *r )
	: _render( r )
{
}

////////////////////////////////////////

painter::~painter( void )
{
}

////////////////////////////////////////

void painter::draw_points( const point *ps, size_t pn )
{
	std::vector<SDL_Point> points( pn );
	for ( size_t i = 0; i < pn; ++i )
	{
		points[i].x = ps[i].x();
		points[i].y = ps[i].y();
	}

	SDL_RenderDrawPoints( _render, points.data(), points.size() );
}

////////////////////////////////////////

void painter::draw_polygon( const point *ps, size_t pn )
{
}

////////////////////////////////////////

void painter::draw_rects( const rectangle *rs, size_t rn )
{
	std::vector<SDL_Rect> rects( rn );
	for ( size_t i = 0; i < rn; ++i )
	{
		rects[i].x = rs[i].x1();
		rects[i].y = rs[i].y1();
		rects[i].w = rs[i].width();
		rects[i].h = rs[i].height();
	}

	SDL_RenderDrawRects( _render, rects.data(), rects.size() );
}

////////////////////////////////////////

void painter::present( void )
{
	SDL_RenderPresent( _render );
}

////////////////////////////////////////

}
