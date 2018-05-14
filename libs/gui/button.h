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

class button_w : public widget
{
public:
	button_w( void );
	button_w( std::string l, base::alignment a = base::alignment::CENTER );
	~button_w( void );

	void set_pressed( bool p );
	void set_text( const std::string &utf8 ) { _text.set_text( utf8 ); }
	void set_color( const gl::color &c ) { _text.set_color( c ); }
	void set_font( const std::shared_ptr<script::font> &f ) { _text.set_font( f ); }
	void set_align( base::alignment a ) { _align = a; }

	void build( context &ogl ) override;
	void paint( context &ogl ) override;

	bool mouse_press( const event &e ) override;
	bool mouse_release( const event &e ) override;
	bool mouse_move( const event &e ) override;

	signal<void(void)> when_activated;

private:
	draw::rectangle _rect;
	draw::text _text;
	base::alignment _align = base::alignment::LEFT;
	bool _pressed = false;
	bool _tracking = false;
};

////////////////////////////////////////

using button = widget_ptr<button_w>;

}

