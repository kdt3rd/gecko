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

class quadrant : public drawable
{
public:
	quadrant( void );

	void create( gl::api &ogl, const path &p, const paint &c );

	void create( gl::api &ogl, const path &p, const gl::color &c )
	{
		create( ogl, p, paint( c, 1.F ) );
	}

	void draw( gl::api &ogl ) override;

	void shape_size( float w, float h )
	{
		_shape.set( w, h );
	}

	void resize( const base::rect &r );

private:
	gl::vec2 _shape;
	gl::vec2 _resize;
	gl::vec2 _top_left;

	gl::mesh _stroke;
	gl::mesh _fill;
	std::shared_ptr<gl::texture> _fill_texture;
	gl::program::uniform _stroke_matrix_loc;
	gl::program::uniform _stroke_shape_loc;
	gl::program::uniform _stroke_resize_loc;
	gl::program::uniform _stroke_topleft_loc;
	gl::program::uniform _fill_matrix_loc;
	gl::program::uniform _fill_shape_loc;
	gl::program::uniform _fill_resize_loc;
	gl::program::uniform _fill_topleft_loc;
};

////////////////////////////////////////

}

