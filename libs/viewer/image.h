
#pragma once

#include "canvas.h"
#include "drawable.h"
#include <gl/program.h>
#include <gl/vertex_array.h>

namespace draw
{

////////////////////////////////////////

class image : public drawable
{
public:
	image( void );
	image( const std::shared_ptr<gl::texture> &t );

	inline void set_texture( const std::shared_ptr<gl::texture> &t )
	{
		_texture = t;
	}

	void draw( gl::context &ctxt ) override;

private:
	float _w = 512, _h = 512;
	std::shared_ptr<gl::texture> _texture;
	std::shared_ptr<gl::program> _prog;
	std::shared_ptr<gl::vertex_array> _quad;
};

////////////////////////////////////////

}

