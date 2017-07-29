//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "drawable.h"
#include "path.h"
#include "paint.h"
#include <gl/mesh.h>
#include <gl/api.h>

namespace draw
{

////////////////////////////////////////

class object : public drawable
{
public:
	object( void );

	void create( gl::api &ogl, const path &p, const paint &c );

	void create( gl::api &ogl, const path &p, const gl::color &c )
	{
		create( ogl, p, paint( c ) );
	}

	void draw( gl::api &ogl ) override;

private:

	gl::mesh _stroke;
	gl::mesh _fill;
	std::shared_ptr<gl::texture> _fill_texture;
	gl::program::uniform _stroke_matrix_loc;
	gl::program::uniform _fill_matrix_loc;
};

////////////////////////////////////////

}

