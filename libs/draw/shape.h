//
// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include "drawable.h"
#include "path.h"
#include "paint.h"
#include "polylines.h"
#include <gl/mesh.h>
#include <gl/api.h>
#include <list>

namespace draw
{

////////////////////////////////////////

class shape : public drawable
{
public:
	shape( void );
	shape( dim x, dim y )
	{
		set_position( x, y );
	}

	void add( gl::api &ogl, const polylines &p, const paint &c );

	void add( gl::api &ogl, const polylines &p, const color &c )
	{
		add( ogl, p, paint( c ) );
	}

	void add( gl::api &ogl, const path &p, const paint &c )
	{
		if ( p.empty() || c.empty() )
			return;

		polylines lines;
		p.replay( lines );
		add( ogl, lines, c );
	}

	void add( gl::api &ogl, const path &p, const color &c )
	{
		add( ogl, p, paint( c ) );
	}

	void clear( void )
	{
		_meshes.clear();
	}

	void rebuild( platform::context &ctxt ) override;
	void draw( platform::context &ctxt ) override;

	void set_position( dim x, dim y )
	{
		_top_left.set( x, y );
	}

	void set_size( dim w, dim h )
	{
		_resize.set( w, h );
	}

	void shape_size( dim w, dim h )
	{
		_shape.set( w, h );
	}

private:
	point _shape = { dim(10), dim(10) };
	point _resize = { dim(10), dim(10) };
	point _top_left = { dim(0), dim(0) };

	struct mesh
	{
		gl::mesh msh;
		std::shared_ptr<gl::texture> tex;
		gl::program::uniform matrix;
	};

	std::list<mesh> _meshes;
};

////////////////////////////////////////

}
