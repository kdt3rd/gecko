//
// Copyright (c) 2014-2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include "scroll_bar.h"
#include <layout/scroll.h>

namespace gui
{

////////////////////////////////////////

class scroll_area : public widget
{
public:
	scroll_area( bool hscroll = true, bool vscroll = true, bool bounded = true );
	~scroll_area( void );

	void set_widget( const std::shared_ptr<widget> &v );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_release( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;

	bool update_layout( double duration ) override;

private:
	void update_widget( void );

	bool _tracking = false;
	point _track;

	std::shared_ptr<layout::scroll> _layout;
	std::shared_ptr<widget> _main;
	std::shared_ptr<widget> _widget;
	std::shared_ptr<scroll_bar> _hscroll;
	std::shared_ptr<scroll_bar> _vscroll;
};

////////////////////////////////////////

}

