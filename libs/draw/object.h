
#pragma once

#include "path.h"
#include "paint.h"
#include <gl/mesh.h>
#include <gl/api.h>

namespace draw
{

////////////////////////////////////////

class object
{
public:
	object( void );

	void create( gl::api &ogl, const path &p, const paint &c );

	void create( gl::api &ogl, const path &p, const gl::color &c )
	{
		create( ogl, p, paint( c ) );
	}

	void draw( gl::api &ogl );

private:
	std::shared_ptr<gl::texture> gradient( gl::api &ogl, const gradient &g, size_t n = 128 );

	gl::mesh _stroke;
	gl::mesh _fill;
	std::shared_ptr<gl::texture> _fill_texture;
	gl::program::uniform _stroke_matrix_loc;
	gl::program::uniform _fill_matrix_loc;
};

////////////////////////////////////////

}

