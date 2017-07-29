//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "gradient.h"
#include <gl/texture.h>
#include <gl/program.h>
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

	std::shared_ptr<gl::program> new_program( gl::api &ogl, const std::string &vert, const std::string &frag );

private:
	static std::map<std::string,std::weak_ptr<gl::program>> _program_cache;
};

////////////////////////////////////////

}

