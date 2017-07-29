//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gl/mesh.h>
#include <gl/color.h>
#include <gl/matrix4.h>
#include "drawable.h"

namespace draw
{

////////////////////////////////////////

class rectangle : public drawable
{
public:
	rectangle( const gl::color &c );
	rectangle( float x, float y, float w, float h, const gl::color &c = gl::white );

	void draw( gl::api &ogl ) override;

	void resize( float x, float y, float w, float h );

	void set_color( const gl::color &c )
	{
		_color = c;
	}

private:
	void initialize( gl::api &ogl );

	gl::matrix4 _rect;
	gl::color _color;

	std::shared_ptr<gl::mesh> _mesh;

	static std::weak_ptr<gl::mesh> _mesh_cache;
	static gl::program::uniform _matrix_loc;
	static gl::program::uniform _color_loc;
};


////////////////////////////////////////

}

