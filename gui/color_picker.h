
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class color_picker : public widget
{
public:
	color_picker( draw::color::space space = draw::color::space::HSL );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	bool mouse_press( const draw::point &p, int b ) override;
	bool mouse_release( const draw::point &p, int b ) override;
	bool mouse_move( const draw::point &p ) override;

private:
	bool _tracking = false;
	draw::color::space _space = draw::color::space::HSL;

	draw::color _current;
};

////////////////////////////////////////

}

