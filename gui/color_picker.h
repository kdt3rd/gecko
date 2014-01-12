
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class color_picker : public widget
{
public:
	color_picker( core::color::space space = core::color::space::HSL );

	void paint( const std::shared_ptr<gldraw::canvas> &c ) override;

	bool mouse_press( const core::point &p, int b ) override;
	bool mouse_release( const core::point &p, int b ) override;
	bool mouse_move( const core::point &p ) override;

private:
	bool _tracking = false;
	core::color::space _space = core::color::space::HSL;

	core::color _current;
};

////////////////////////////////////////

}

