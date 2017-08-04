//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "drawable.h"
#include <script/font.h>
#include <gl/color.h>
#include <gl/mesh.h>

namespace draw
{

////////////////////////////////////////

class text : public drawable
{
public:
	text( void );
	text( const std::shared_ptr<script::font> &font );
	text( const std::shared_ptr<script::font> &font, const std::string &utf8 );

	void set_font( const std::shared_ptr<script::font> &font );
	void set_text( const std::string &utf8 );
	void set_position( const gl::vec2 &p );
	void set_position( const base::point &p )
	{
		set_position( gl::vec2( p.x(), p.y() ) );
	}
	void set_color( const gl::color &c );

	const std::shared_ptr<script::font> &get_font( void ) const
	{
		return _font;
	}

	const std::string &get_text( void ) const
	{
		return _utf8;
	}

	std::string &get_text( void )
	{
		return _utf8;
	}

	void draw( gl::api &ogl ) override;

private:
	void update( void );

	bool _update = false;

	std::shared_ptr<script::font> _font;
	std::string _utf8;
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

	static std::weak_ptr<gl::program> _program_cache;
	static std::map<std::shared_ptr<script::font>, GlyphPack> _font_glyph_cache;
};

////////////////////////////////////////

}
