
#pragma once

#include "drawable.h"
#include "canvas.h"
#include <gl/api.h>
#include <base/point.h>

namespace draw
{

////////////////////////////////////////

class color_wheel : public drawable
{
public:
	color_wheel( void );

	void create( const std::shared_ptr<canvas> &c, const base::point &center, double radius );

	void draw( gl::api &ogl ) override;

private:
	std::shared_ptr<gl::vertex_array> _data;
	size_t _data_size = 0;
	std::vector<std::tuple<gl::primitive,size_t,size_t>> _fill_parts;
	std::shared_ptr<gl::program> _fill_prog;
};

////////////////////////////////////////

}
