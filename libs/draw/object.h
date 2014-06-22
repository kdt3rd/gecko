
#pragma once

#include "canvas.h"
#include "drawable.h"

namespace draw
{

////////////////////////////////////////

class object : public drawable
{
public:
	object( void );

	void create( const std::shared_ptr<canvas> &c, const core::path &path, const core::paint &paint );

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

