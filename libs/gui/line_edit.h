//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include "application.h"
#include "style.h"
#include <draw/stretchable.h>
#include <base/alignment.h>

namespace gui
{

////////////////////////////////////////

class line_edit : public widget
{
public:
	line_edit( void );
	line_edit( std::string l, base::alignment a = base::alignment::LEFT, const base::color &c = { 0, 0, 0, 1 }, const std::shared_ptr<script::font> &f = application::current()->get_default_font() );
	~line_edit( void );

	const std::string &text( void ) const { return _text; }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<script::font> &f ) { _font = f; }
	void set_align( base::alignment a ) { _align = a; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool key_press( platform::scancode c ) override;

	bool text_input( char32_t c ) override;

private:
	size_t _cursor = 0;
	std::string _text;
	base::alignment _align = base::alignment::LEFT;
	base::color _color = { 0, 0, 0, 1 };
	std::shared_ptr<script::font> _font = application::current()->get_default_font();

	std::shared_ptr<draw::stretchable> _frame;
	std::shared_ptr<draw::stretchable> _marker;
};

////////////////////////////////////////

}

