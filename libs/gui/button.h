//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include "application.h"
#include <draw/rectangle.h>
#include <draw/text.h>
#include <base/alignment.h>
#include <base/signal.h>

namespace gui
{

////////////////////////////////////////

class button : public widget
{
public:
	button( void );
	button( std::string l, base::alignment a = base::alignment::CENTER, const std::shared_ptr<script::font> &f = application::current()->get_default_font() );
	~button( void );

	void set_pressed( bool p );
	void set_text( const std::string &utf8 ) { _text.set_text( utf8 ); }
	void set_color( const gl::color &c ) { _text.set_color( c ); }
	void set_font( const std::shared_ptr<script::font> &f ) { _text.set_font( f ); }
	void set_align( base::alignment a ) { _align = a; }

	void build( gl::api &ogl ) override;

	void paint( gl::api &ogl ) override;

	void compute_bounds( void ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_release( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;

	base::signal<void(void)> when_activated;

private:
	draw::rectangle _rect;
	draw::text _text;
	base::alignment _align = base::alignment::LEFT;
	bool _pressed = false;
	bool _tracking = false;
};

////////////////////////////////////////

}

