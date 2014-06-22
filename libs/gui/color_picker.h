
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class color_picker : public widget
{
public:
	color_picker( base::color::space space = base::color::space::HSL );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	bool mouse_press( const base::point &p, int b ) override;
	bool mouse_release( const base::point &p, int b ) override;
	bool mouse_move( const base::point &p ) override;

private:
	bool _tracking = false;
	base::color::space _space = base::color::space::HSL;

	base::color _current;
};

////////////////////////////////////////

}

