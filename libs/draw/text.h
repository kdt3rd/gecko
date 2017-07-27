//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <script/font.h>
#include <gl/color.h>
#include <gl/mesh.h>

namespace draw
{

////////////////////////////////////////

class text
{
public:
	text( const std::shared_ptr<script::font> &font );

	void set_text( const std::string &utf8 );
	void set_position( const gl::vec2 &p );
	void set_color( const gl::color &c );

	void draw( gl::api &ogl );

private:
	void update( void );

	bool _update = false;

	std::string _utf8;
	std::shared_ptr<script::font> _font;
	gl::vec2 _pos;
	gl::color _color;

	gl::mesh _mesh;
	gl::program::uniform _col_pos;
	gl::program::uniform _tex_pos;
	gl::program::uniform _mat_pos;
	struct GlyphPack
	{
		uint32_t version;
		std::shared_ptr<gl::texture> texture;
	};
	static std::map<std::shared_ptr<script::font>, GlyphPack> _font_glyph_cache;
};

////////////////////////////////////////

}
