//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include "application.h"
#include <draw/text.h>
#include <draw/rectangle.h>

namespace gui
{

////////////////////////////////////////

class line_edit_w : public widget
{
public:
	line_edit_w( void );
	line_edit_w( std::string l );
	~line_edit_w( void );

	const std::string &text( void ) const { return _text.get_text(); }
	void set_text( const std::string &t ) { _text.set_text( t ); }
	void set_prompt( const std::string &p ) { _prompt.set_text( p ); }

	void set_font( std::shared_ptr<script::font> &f ) { _text.set_font( f ); _prompt.set_font( f ); }

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool key_press( platform::scancode c ) override;

	bool text_input( char32_t c ) override;

	bool mouse_press( const point &p, int button ) override;

private:
	draw::rectangle _line;
	draw::rectangle _marker;
	draw::text _text;
	draw::text _prompt;
	size_t _cursor = 0;
};

////////////////////////////////////////

using line_edit = widget_ptr<line_edit_w>;

}

