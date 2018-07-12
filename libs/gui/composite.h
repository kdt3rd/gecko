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
	~composite( void ) override;

	void monitor_changed( context &ctxt ) override;
	void build( context &ctxt ) override;
	void paint( context &ctxt ) override;

	std::shared_ptr<widget> find_widget_under( const point &p ) override;

	bool update_layout( double duration ) override;

protected:
	std::shared_ptr<widget> _mouse_grab;

	virtual void for_subwidgets( const std::function<void(const std::shared_ptr<widget> &)> &f ) = 0;
};

////////////////////////////////////////

}
