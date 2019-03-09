// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <gl/color.h>
#include <string>
#include <array>
#include <vector>

////////////////////////////////////////

namespace draw
{
	using palette = std::array<gl::color,10>;

	const std::vector<std::string> &palette_names( void );
	const palette &get_palette( const std::string &name );

	extern const palette red;
	extern const palette pink;
	extern const palette purple;
	extern const palette deep_purple;
	extern const palette indigo;
	extern const palette blue;
	extern const palette light_blue;
	extern const palette cyan;
	extern const palette teal;
	extern const palette green;
	extern const palette light_green;
	extern const palette lime;
	extern const palette yellow;
	extern const palette amber;
	extern const palette orange;
	extern const palette deep_orange;
	extern const palette brown;
	extern const palette blue_grey;
	extern const palette grey;
}

////////////////////////////////////////
