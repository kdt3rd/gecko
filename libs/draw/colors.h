//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gl/color.h>
#include <string>
#include <array>
#include <vector>

////////////////////////////////////////

namespace draw
{
	const std::vector<std::string> &palette_names( void );
	const std::array<gl::color,10> &palette( const std::string &name );

	extern const std::array<gl::color,10> red;
	extern const std::array<gl::color,10> pink;
	extern const std::array<gl::color,10> purple;
	extern const std::array<gl::color,10> deep_purple;
	extern const std::array<gl::color,10> indigo;
	extern const std::array<gl::color,10> blue;
	extern const std::array<gl::color,10> light_blue;
	extern const std::array<gl::color,10> cyan;
	extern const std::array<gl::color,10> teal;
	extern const std::array<gl::color,10> green;
	extern const std::array<gl::color,10> light_green;
	extern const std::array<gl::color,10> lime;
	extern const std::array<gl::color,10> yellow;
	extern const std::array<gl::color,10> amber;
	extern const std::array<gl::color,10> orange;
	extern const std::array<gl::color,10> deep_orange;
	extern const std::array<gl::color,10> brown;
	extern const std::array<gl::color,10> blue_grey;
	extern const std::array<gl::color,10> grey;
}

////////////////////////////////////////
