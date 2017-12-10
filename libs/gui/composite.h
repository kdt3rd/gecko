//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class composite : public widget
{
public:
	using widget::widget;

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_release( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;
	bool mouse_wheel( int amount ) override;
	bool key_press( platform::scancode c ) override;
	bool key_release( platform::scancode c ) override;
	bool text_input( char32_t c ) override;

	bool update_layout( double duration ) override;

protected:
	std::shared_ptr<widget> _mouse_grab;

	virtual void for_subwidgets( const std::function<void(const std::shared_ptr<widget> &)> &f ) = 0;
};

////////////////////////////////////////

}

