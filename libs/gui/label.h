//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include "widget.h"
#include "application.h"
#include <gl/color.h>
#include <draw/text.h>
#include <draw/rectangle.h>

namespace gui
{

////////////////////////////////////////

class label_w : public widget
{
public:
	label_w( void );
	label_w( std::string l, base::alignment a = alignment::LEFT );
	~label_w( void ) override;

	void set_text( const std::string &utf8 ) { _text.set_text( utf8 ); }
	void set_color( const color &c ) { _text.set_color( c ); }
	void set_font( const std::shared_ptr<script::font> &f ) { _text.set_font( f ); }
	void set_align( alignment a ) { _align = a; }
	void set_bg( const color &c )
	{
		_bg_color = c;
		_bg.set_color( c );
	}

	void build( context &ctxt ) override;

	void paint( context &ctxt ) override;

private:
	color _bg_color;
	draw::rectangle _bg;
	draw::text _text;
	alignment _align = alignment::LEFT;
};

////////////////////////////////////////

using label = widget_ptr<label_w>;

}
