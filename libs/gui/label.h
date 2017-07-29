//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include "application.h"
#include <base/alignment.h>
#include <gl/color.h>
#include <draw/text.h>

namespace gui
{

////////////////////////////////////////

class label : public widget
{
public:
	label( void );
	label( std::string l, base::alignment a = base::alignment::LEFT, const gl::color &c = gl::white, const std::shared_ptr<script::font> &f = application::current()->get_default_font() );
	~label( void );

	void set_text( const std::string &utf8 ) { _text.set_text( utf8 ); }
	void set_color( const gl::color &c ) { _text.set_color( c ); }
	void set_font( const std::shared_ptr<script::font> &f ) { _text.set_font( f ); }
	void set_align( base::alignment a ) { _align = a; }

	void paint( gl::api &ogl ) override;

	void compute_bounds( void ) override;

private:
	draw::text _text;
	base::alignment _align = base::alignment::LEFT;
};

////////////////////////////////////////

}

