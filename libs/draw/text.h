
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

	void render( void );

private:
	void update( void );

	bool _update = false;

	std::string _utf8;
	std::shared_ptr<script::font> _font;
	gl::vec2 _pos;
	gl::color _color;

	gl::mesh _mesh;

	struct GlyphPack
	{
		uint32_t version;
		std::shared_ptr<gl::texture> texture;
	};
	static std::map<std::shared_ptr<script::font>, GlyphPack> _font_glyph_cache;
};

////////////////////////////////////////

}
