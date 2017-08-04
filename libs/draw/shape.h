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
	shape( float x, float y )
	{
		set_position( x, y );
	}

	void add( gl::api &ogl, const polylines &p, const paint &c );

	void add( gl::api &ogl, const polylines &p, const gl::color &c )
	{
		add( ogl, p, paint( c, 1.F ) );
	}

	void add( gl::api &ogl, const path &p, const paint &c )
	{
		if ( p.empty() || c.empty() )
			return;

		polylines lines;
		p.replay( lines );
		add( ogl, lines, c );
	}

	void add( gl::api &ogl, const path &p, const gl::color &c )
	{
		add( ogl, p, paint( c, 1.F ) );
	}

	void clear( void )
	{
		_meshes.clear();
	}

	void draw( gl::api &ogl ) override;

	void set_position( float x, float y )
	{
		_top_left.set( x, y );
	}

	void set_size( float w, float h )
	{
		_resize.set( w, h );
	}

	void shape_size( float w, float h )
	{
		_shape.set( w, h );
	}

private:
	gl::vec2 _shape = { 10.F, 10.F };
	gl::vec2 _resize = { 10.F, 10.F };
	gl::vec2 _top_left = { 0.F, 0.F };

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

