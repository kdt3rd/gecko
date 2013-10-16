
#pragma once

#include <stddef.h>
#include <core/point.h>
#include <core/line.h>
#include <core/rectangle.h>
#include <core/color.h>

namespace platform
{

////////////////////////////////////////

class painter
{
public:
	painter( void );
	virtual ~painter( void );

	virtual void set_color( const color &c ) = 0;

	virtual void draw_points( const point *ls, size_t ln ) = 0;
	virtual void draw_polygon( const point *ps, size_t pn ) = 0;

	virtual void draw_lines( const line *ps, size_t pn ) = 0;
	virtual void draw_rects( const rectangle *rs, size_t rn ) = 0;

	virtual void clear( void ) =0 ;

	virtual void present( void ) = 0;
};

////////////////////////////////////////

}
