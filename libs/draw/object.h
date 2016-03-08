
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

	void create( const std::shared_ptr<canvas> &c, const base::path &path, const base::paint &paint );

	inline void create( const std::shared_ptr<canvas> &c, const base::path &path, const base::color &color )
	{
		base::paint p;
		p.set_fill_color( color );
		create( c, path, p );
	}


	void draw( gl::api &ogl ) override;

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

