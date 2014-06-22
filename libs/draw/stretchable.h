
#pragma once

#include "canvas.h"
#include "drawable.h"

namespace draw
{

////////////////////////////////////////

class stretchable : public drawable
{
public:
	stretchable( void );

	void create( const std::shared_ptr<canvas> &c, const base::path &path, const base::paint &paint, const base::point &center );

	void set( const std::shared_ptr<canvas> &c, const base::rect &r );

	void draw( gl::context &ctxt ) override;

private:
	std::shared_ptr<gl::vertex_array> _stroke;
	std::vector<std::tuple<gl::primitive,size_t,size_t>> _stroke_parts;
	std::shared_ptr<gl::program> _stroke_prog;

	std::shared_ptr<gl::vertex_array> _fill;
	std::vector<std::tuple<gl::primitive,size_t,size_t>> _fill_parts;
	std::shared_ptr<gl::texture> _fill_texture;
	std::shared_ptr<gl::program> _fill_prog;
};

////////////////////////////////////////

}

