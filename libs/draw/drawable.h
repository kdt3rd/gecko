//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "paint.h"
#include "gradient.h"
#include <gl/texture.h>
#include <gl/program.h>
#include <gl/mesh.h>
#include <map>
#include <string>

namespace draw
{

////////////////////////////////////////

class drawable
{
public:
	virtual ~drawable( void );

	virtual void draw( gl::api &ogl ) = 0;

protected:
	std::shared_ptr<gl::texture> new_gradient( gl::api &ogl, const gradient &g, size_t n = 128 );

	std::shared_ptr<gl::program> new_program( gl::api &ogl, const std::string &vert, const std::string &frag, bool cached = true );

	std::shared_ptr<gl::texture> get_fill_texture( gl::api &ogl, const paint &p );
	gl::program::uniform fill_mesh( gl::api &ogl, gl::mesh &m, const paint &p, const std::string &vert = "position_uv.vert" );
	gl::program::uniform stroke_mesh( gl::api &ogl, gl::mesh &m, const paint &p );

private:
	static std::map<std::string,std::weak_ptr<gl::program>> _program_cache;
};

////////////////////////////////////////

}

