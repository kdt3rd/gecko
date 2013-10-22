
#pragma once

#include <stddef.h>
#include <memory>
#include <draw/color.h>
#include "rectangles.h"
#include "points.h"

namespace platform
{

////////////////////////////////////////

class painter
{
public:
	painter( void );
	virtual ~painter( void );

	virtual void set_color( const draw::color &c ) = 0;

	virtual std::shared_ptr<points> new_points( void ) = 0;

	virtual void draw_points( const std::shared_ptr<points> &ls ) = 0;

	virtual void draw_polygon( const std::shared_ptr<points> &ps ) = 0;
	virtual void fill_polygon( const std::shared_ptr<points> &ps ) = 0;

//	virtual void draw_lines( const line *ps, size_t pn ) = 0;

	virtual std::shared_ptr<rectangles> new_rectangles( void ) = 0;
	virtual void draw_rects( const std::shared_ptr<rectangles> &rs ) = 0;
	virtual void fill_rects( const std::shared_ptr<rectangles> &rs ) = 0;

//	virtual void draw_arc( const rectangle *rs, size_t rn ) = 0;

	std::shared_ptr<points> make_star( double x, double y, double radius, int npoints, int density = 2 );

	virtual void clear( void ) =0 ;

	virtual void present( void ) = 0;
};

////////////////////////////////////////

}

