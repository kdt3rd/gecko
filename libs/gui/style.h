//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gl/color.h>
#include <draw/colors.h>
#include <script/font_manager.h>
#include "types.h"

namespace gui
{

////////////////////////////////////////

class style
{
public:
	size widget_minimum_size( void ) const
	{
		return _min_widget_size;
	}

	color background_color( void ) const
	{
		return _bg;
	}

	color dominant_light( void ) const
	{
		return _dominant[0];
	}

	color dominant_color( void ) const
	{
		return _dominant[1];
	}

	color dominant_dark( void ) const
	{
		return _dominant[2];
	}

	color accent_color( void ) const
	{
		return _accent;
	}

	color primary_text( color bg ) const;
	color secondary_text( color bg ) const;
	color disabled_text( color bg ) const;

	color divider( color bg ) const;

	color active_icon( color bg ) const;
	color inactive_icon( color bg ) const;

	const std::shared_ptr<script::font> &display_font( void ) const
	{
		return _display;
	}

	const std::shared_ptr<script::font> &title_font( void ) const
	{
		return _title;
	}

	const std::shared_ptr<script::font> &body_font( void ) const
	{
		return _body;
	}

	const std::shared_ptr<script::font> &caption_font( void ) const
	{
		return _caption;
	}

	size_t version( void ) const
	{
		return _version;
	}

	void set_font_manager( const std::shared_ptr<script::font_manager> &fmgr );

	void set_background_color( const color &c )
	{
		_bg = c;
		++_version;
	}

	void set_dominant_light( const color &c )
	{
		_dominant[0] = c;
		++_version;
	}

	void set_dominant_normal( const color &c )
	{
		_dominant[1] = c;
		++_version;
	}

	void set_dominant_dark( const color &c )
	{
		_dominant[2] = c;
		++_version;
	}

	void set_accent_color( const color &c )
	{
		_accent = c;
		++_version;
	}

private:
	size_t _version = 0;
	color _bg = gl::white;
	color _dominant[3] = { draw::indigo[2], draw::indigo[5], draw::indigo[9] };
	color _accent = draw::orange[5];

	size _min_widget_size{ 4.F, 4.F };

	std::shared_ptr<script::font_manager> _fmgr;

	std::shared_ptr<script::font> _display;
	std::shared_ptr<script::font> _title;
	std::shared_ptr<script::font> _body;
	std::shared_ptr<script::font> _caption;
};

////////////////////////////////////////

}

