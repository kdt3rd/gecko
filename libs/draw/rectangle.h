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
	rectangle( const gl::color &c = gl::white );
	rectangle( float x, float y, float w, float h, const gl::color &c = gl::white );

	void rebuild( platform::context &ctxt ) override;
	void draw( platform::context &ctxt ) override;

	void set_size( float w, float h );
	void set_position( float x, float y );

	void set_color( const gl::color &c )
	{
		_color = c;
	}

private:
	void initialize( platform::context &ctxt );

	gl::matrix4 _rect;
	gl::color _color;

	struct cache_entry
	{
		gl::mesh _mesh;
		gl::program::uniform _matrix_loc;
		gl::program::uniform _color_loc;
	};

	std::shared_ptr<cache_entry> _stash;
};


////////////////////////////////////////

}

