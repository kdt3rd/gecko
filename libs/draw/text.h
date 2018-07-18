//
// Copyright (c) 2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "drawable.h"
#include <script/font.h>
#include <gl/mesh.h>

namespace draw
{

////////////////////////////////////////

class text : public drawable
{
public:
	text( void );
	text( const std::shared_ptr<script::font> &font );
	text( const std::string &utf8 );
	text( const std::shared_ptr<script::font> &font, const std::string &utf8 );

	void set_font( const std::shared_ptr<script::font> &font );
	void set_text( const std::string &utf8 );
	void set_scale( const point &s );
	void set_position( const point &p );
	void set_position( dim x, dim y )
	{
		set_position( point( x, y ) );
	}
	template <typename T>
	void set_position( const base::point<T, 2> &p )
	{
		set_position( point( static_cast<dim>( p[0] ), static_cast<dim>( p[1] ) ) );
	}
	void set_color( const color &c );

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

	void rebuild( platform::context &ctxt ) override;
	void draw( platform::context &ctxt ) override;

private:
	void update( platform::context &ctxt );

	bool _update = false;

	std::shared_ptr<script::font> _font;
	std::string _utf8;
	point _pos;
	point _scale;
	color _color;

	gl::mesh _mesh;
	gl::program::uniform _col_pos;
	gl::program::uniform _tex_pos;
	gl::program::uniform _mat_pos;

	struct GlyphPack
	{
		uint32_t version;
		std::shared_ptr<gl::texture> texture;
	};

	struct cache_entry
	{
		std::shared_ptr<gl::program> _program_cache;
		std::map<std::shared_ptr<script::font>, GlyphPack> _font_glyph_cache;
	};
	std::shared_ptr<cache_entry> _stash;
};

////////////////////////////////////////

}
